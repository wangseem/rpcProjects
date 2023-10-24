/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:20
 */
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/tcp_client.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/fd_event_group.h"
#include "rocket/common/error_code.h"
#include "rocket/net/tcp/net_addr.h"

namespace rocket
{
    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:03:12
     * @Description: 构造函数，主要是创建socket，获取当前的io线程，根据connection的构造函数的参数创建一个智能指针
     * @param {s_ptr} peer_addr
     * @return {*}
     */
    TcpClient::TcpClient(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr)
    {
        m_event_loop = EventLoop::GetCurrentEventLoop();
        m_fd = socket(peer_addr->getFamily(), SOCK_STREAM, 0);

        if (m_fd < 0)
        {
            ERRORLOG("TcpClient::TcpClient() error, failed to create fd");
            return;
        }

        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(m_fd);
        m_fd_event->setNonBlock();//设置非阻塞

        m_connection = std::make_shared<TcpConnection>(m_event_loop, m_fd, 128, peer_addr, nullptr, TcpConnectionByClient);
        m_connection->setConnectionType(TcpConnectionByClient);//默认使用客户端
    }
 
    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:03:01
     * @Description: //析构函数关闭fd；
     * @return {*}
     */    
    TcpClient::~TcpClient()
    {
        DEBUGLOG("TcpClient::~TcpClient()");
        if (m_fd > 0)
        {
            close(m_fd);
        }
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-18 13:28:53
     * @Description:  // 异步的进行 conenct
     * // 如果connect 成功，done 会被执行
     * @return {*}
     */
    void TcpClient::connect(std::function<void()> done)
    {

        // connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
        int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen()); // 客户端connect （fd，服务端地址就是sockaddr_in，长度）
        if (rt == 0) //成功
        {
            DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
            m_connection->setState(Connected);//设置连接状态
            initLocalAddr();
            if (done)
            {
                done();//执行回调函数
            }
        }
        else if (rt == -1)//说明已经连接过了，或者出现错误了，如果errno == einprogress 那就说明他是非
        {
            if (errno == EINPROGRESS)//刚好我设置的非阻塞，调用connect接口不能立马建立连接，所以就会报这个错误。
            {
                // epoll 监听可写事件，然后判断错误码
                //done是回调函数，捕获的是回调函数和this指针
                m_fd_event->listen(FdEvent::OUT_EVENT,[this, done]()
                {
                    int rt = ::connect(m_fd, m_peer_addr->getSockAddr(), m_peer_addr->getSockLen());
                    if ((rt < 0 && errno == EISCONN) || (rt == 0))//已被连接
                    {
                        DEBUGLOG("connect [%s] sussess", m_peer_addr->toString().c_str());
                        initLocalAddr();
                        m_connection->setState(Connected);
                    }
                    else
                    {
                        if (errno == ECONNREFUSED)//拒绝连接
                        {
                            m_connect_error_code = ERROR_PEER_CLOSED;
                            m_connect_error_info = "connect refused, sys error = " + std::string(strerror(errno));
                        }
                        else//否则就是连接失败，我们也不知道原因
                        {
                            m_connect_error_code = ERROR_FAILED_CONNECT;
                            m_connect_error_info = "connect unkonwn error, sys error = " + std::string(strerror(errno));
                        }
                        ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                        close(m_fd);//关闭原来的套接字
                        m_fd = socket(m_peer_addr->getFamily(), SOCK_STREAM, 0);//重新申请一下
                    }

                    // 连接完后需要去掉可写事件的监听，不然会一直触发
                    m_event_loop->deleteEpollEvent(m_fd_event);
                    DEBUGLOG("now begin to done");
                    // 如果连接完成，才会执行回调函数
                    if (done)
                    {
                        done();
                    }
                });
                m_event_loop->addEpollEvent(m_fd_event);//添加eventloop；
                //判断loop是否打开
                if (!m_event_loop->isLooping())
                {
                    m_event_loop->loop();
                }
            }
            else//说明是其他的问题，
            {
                ERRORLOG("connect errror, errno=%d, error=%s", errno, strerror(errno));
                m_connect_error_code = ERROR_FAILED_CONNECT;
                m_connect_error_info = "connect error, sys error = " + std::string(strerror(errno));
                if (done)
                {
                    done();
                }
            }
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:07:52
     * @Description: 关闭io线程
     * @return {*}
     */
    void TcpClient::stop()
    {
        if (m_event_loop->isLooping())
        {
            m_event_loop->stop();
        }
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:46:40
     * @Description: // 异步的发送 message
     * // 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
     * @return {*}
     */
    
    void TcpClient::writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1. 把 message 对象写入到 Connection 的 buffer, done 也写入
        // 2. 启动 connection 可写事件
        m_connection->pushSendMessage(message, done);
        m_connection->listenWrite();//启动监听可写事件
    }

    // 异步的读取 message
    // 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
    void TcpClient::readMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        // 1. 监听可读事件
        // 2. 从 buffer 里 decode 得到 message 对象, 判断是否 msg_id 相等，相等则读成功，执行其回调
        m_connection->pushReadMessage(msg_id, done);
        m_connection->listenRead();//启动监听可读事件
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-25 22:55:19
     * @Description: 获取错误信息
     * @return {*}
     */    
    int TcpClient::getConnectErrorCode()
    {
        return m_connect_error_code;
    }

    std::string TcpClient::getConnectErrorInfo()
    {
        return m_connect_error_info;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-25 22:55:32
     * @Description: 获取地址
     * @return {*}
     */    
    NetAddr::s_ptr TcpClient::getPeerAddr()
    {
        return m_peer_addr;
    }

    NetAddr::s_ptr TcpClient::getLocalAddr()
    {
        return m_local_addr;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:01:42
     * @Description:获取fd所绑定的四元组 
     * @return {*}
     */    
    void TcpClient::initLocalAddr()
    {
        sockaddr_in local_addr;
        socklen_t len = sizeof(local_addr);

        int ret = getsockname(m_fd, reinterpret_cast<sockaddr *>(&local_addr), &len);//把sock绑定的ip地址和返回给local_addr
        if (ret != 0)
        {
            ERRORLOG("initLocalAddr error, getsockname error. errno=%d, error=%s", errno, strerror(errno));
            return;
        }

        m_local_addr = std::make_shared<IPNetAddr>(local_addr);//去解析这个addr
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-18 14:02:10
     * @Description: 添加定时器定时器任务
     * @param {s_ptr} timer_event
     * @return {*}
     */
    void TcpClient::addTimerEvent(TimerEvent::s_ptr timer_event)
    {
        m_event_loop->addTimerEvent(timer_event);
    }

}