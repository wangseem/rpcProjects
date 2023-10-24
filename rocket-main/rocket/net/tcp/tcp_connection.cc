#include <unistd.h>
#include "rocket/common/log.h"
#include "rocket/net/fd_event_group.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/coder/string_coder.h"
#include "rocket/net/coder/tinypb_coder.h"

namespace rocket
{
    /**
     * @Author: kevin
     * @Date: 2023-09-06 14:39:39
     * @Description: 构造函数
     * @param {EventLoop} *event_loop
     * @param {int} fd
     * @param {int} buffer_size
     * @param {s_ptr} peer_addr
     * @param {s_ptr} local_addr
     * @param {TcpConnectionType} type
     * @return {*}
     */    
    TcpConnection::TcpConnection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, NetAddr::s_ptr local_addr, TcpConnectionType type /*= TcpConnectionByServer*/)
        : m_event_loop(event_loop), m_local_addr(local_addr), m_peer_addr(peer_addr), m_state(NotConnected), m_fd(fd), m_connection_type(type)
    {

        m_in_buffer = std::make_shared<TcpBuffer>(buffer_size);  //读缓冲区
        m_out_buffer = std::make_shared<TcpBuffer>(buffer_size); //写缓冲区

        m_fd_event = FdEventGroup::GetFdEventGroup()->getFdEvent(fd);
        m_fd_event->setNonBlock();//套接字设置为非阻塞；

        m_coder = new TinyPBCoder();

        if (m_connection_type == TcpConnectionByServer)
        {
            listenRead();
        }
    }
    
    TcpConnection::~TcpConnection()
    {
        DEBUGLOG("~TcpConnection");
        if (m_coder)
        {
            delete m_coder;
            m_coder = NULL;
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-16 22:52:05
     * @Description: 读取客户端发来的数据组装为RPC请求
     * @return {*}
     */    
    void TcpConnection::onRead()
    {
        // 1. 从 socket 缓冲区，调用 系统的 read 函数读取字节 in_buffer 里面
        if (m_state != Connected)
        {
            ERRORLOG("onRead error, client has already disconneced, addr[%s], clientfd[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }

        //
        bool is_read_all = false;
        bool is_close = false;
        while (!is_read_all)
        {
            if (m_in_buffer->writeAble() == 0)
            {
                m_in_buffer->resizeBuffer(2 * m_in_buffer->m_buffer.size());//扩容两倍
            }
            int read_count = m_in_buffer->writeAble();//所读的值就是我的缓冲区能写的大小
            int write_index = m_in_buffer->writeIndex();

            int rt = read(m_fd, &(m_in_buffer->m_buffer[write_index]), read_count);
            DEBUGLOG("success read %d bytes from addr[%s], client fd[%d]", rt, m_peer_addr->toString().c_str(), m_fd);
            if (rt > 0)
            {
                m_in_buffer->moveWriteIndex(rt);
                if (rt == read_count)
                {
                    continue;
                }
                else if (rt < read_count)
                {
                    is_read_all = true;
                    break;
                }
            }
            else if (rt == 0)
            {
                is_close = true;
                break;
            }
            else if (rt == -1 && errno == EAGAIN)
            {
                is_read_all = true;
                break;
            }
        }

        if (is_close)
        {
            // TODO:
            INFOLOG("peer closed, peer addr [%s], clientfd [%d]", m_peer_addr->toString().c_str(), m_fd);
            clear();//清理链接
            return;
        }

        if (!is_read_all)
        {
            ERRORLOG("not read all data");
        }

        // TODO: 简单的 echo, 后面补充 RPC 协议解析
        excute();//解密
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-16 22:51:48
     * @Description:解析读取的数据 
     * @return {*}
     */    
    void TcpConnection::excute()
    {
        if (m_connection_type == TcpConnectionByServer)//作为服务端使用，代表跟对端客户端的连接
        {
            // 将 RPC 请求执行业务逻辑，获取 RPC 响应, 再把 RPC 响应发送回去
            std::vector<AbstractProtocol::s_ptr> result;
            m_coder->decode(result, m_in_buffer);
            for (size_t i = 0; i < result.size(); ++i)
            {
                // 1. 针对每一个请求，调用 rpc 方法，获取响应 message
                // 2. 将响应 message 放入到发送缓冲区，监听可写事件回包
                INFOLOG("success get request[%s] from client[%s]", result[i]->m_msg_id.c_str(), m_peer_addr->toString().c_str());

                std::shared_ptr<TinyPBProtocol> message = std::make_shared<TinyPBProtocol>();
                // message->m_pb_data = "hello. this is rocket rpc test data";
                // message->m_msg_id = result[i]->m_msg_id;

                RpcDispatcher::GetRpcDispatcher()->dispatch(result[i], message, this);//tcpconnection和dispatch结合起来了
            }
        }
        else
        {
            // 从 buffer 里 decode 得到 message 对象, 执行其回调
            std::vector<AbstractProtocol::s_ptr> result;
            m_coder->decode(result, m_in_buffer);

            for (size_t i = 0; i < result.size(); ++i)
            {
                std::string msg_id = result[i]->m_msg_id;
                auto it = m_read_dones.find(msg_id);
                if (it != m_read_dones.end())
                {
                    it->second(result[i]);
                    m_read_dones.erase(it);
                }
            }
        }
    }

    void TcpConnection::reply(std::vector<AbstractProtocol::s_ptr> &replay_messages)
    {
        m_coder->encode(replay_messages, m_out_buffer);
        listenWrite();
    }

    void TcpConnection::onWrite()
    {
        // 将当前 m_out_buffer 里面的数据全部发送给 client

        if (m_state != Connected)//如果不是连接就报错
        {
            ERRORLOG("onWrite error, client has already disconneced, addr[%s], clientfd[%d]", m_peer_addr->toString().c_str(), m_fd);
            return;
        }

        if (m_connection_type == TcpConnectionByClient)//作为客户端使用，代表跟对端服务端的连接
        {
            //  1. 将 message encode 得到字节流
            // 2. 将字节流入到 buffer 里面，然后全部发送

            std::vector<AbstractProtocol::s_ptr> messages;

            for (size_t i = 0; i < m_write_dones.size(); ++i)
            {
                messages.push_back(m_write_dones[i].first);
            }

            m_coder->encode(messages, m_out_buffer);
        }

        bool is_write_all = false;
        while (true)
        {
            if (m_out_buffer->readAble() == 0)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            int write_size = m_out_buffer->readAble();
            int read_index = m_out_buffer->readIndex();

            int rt = write(m_fd, &(m_out_buffer->m_buffer[read_index]), write_size);

            if (rt >= write_size)
            {
                DEBUGLOG("no data need to send to client [%s]", m_peer_addr->toString().c_str());
                is_write_all = true;
                break;
            }
            if (rt == -1 && errno == EAGAIN)
            {
                // 发送缓冲区已满，不能再发送了。
                // 这种情况我们等下次 fd 可写的时候再次发送数据即可
                ERRORLOG("write data error, errno==EAGIN and rt == -1");
                break;
            }
        }
        if (is_write_all)
        {
            m_fd_event->cancle(FdEvent::OUT_EVENT);//取消可写事件的监听
            m_event_loop->addEpollEvent(m_fd_event);//更新fd
        }

        if (m_connection_type == TcpConnectionByClient)
        {
            for (size_t i = 0; i < m_write_dones.size(); ++i)
            {
                m_write_dones[i].second(m_write_dones[i].first);
            }
            m_write_dones.clear();
        }
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-16 23:24:03
     * @Description: 设置tcp状态信息
     * @param {TcpState} state
     * @return {*}
     */    
    void TcpConnection::setState(const TcpState state)
    {
        m_state = Connected;
    }
    
    /**
     * @Author: kevin
     * @Date: 2023-09-16 23:24:22
     * @Description:获取tcp状态 
     * @return {*}
     */    
    TcpState TcpConnection::getState()
    {
        return m_state;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-16 22:47:32
     * @Description: 清理连接
     * @return {*}
     */
    void TcpConnection::clear()
    {
        // 处理一些关闭连接后的清理动作
        if (m_state == Closed)//判断状态
        {
            return;
        }
        m_fd_event->cancle(FdEvent::IN_EVENT);
        m_fd_event->cancle(FdEvent::OUT_EVENT);

        m_event_loop->deleteEpollEvent(m_fd_event);//将fd在eventloop里删除掉

        m_state = Closed;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-17 00:01:12
     * @Description: 服务器主动关闭连接
     * @return {*}
     */    
    void TcpConnection::shutdown()
    {
        if (m_state == Closed || m_state == NotConnected)//判断状态
        {
            return;
        }

        // 处于半关闭
        m_state = HalfClosing;

        // 调用 shutdown 关闭读写，意味着服务器不会再对这个 fd 进行读写操作了
        // 发送 FIN 报文， 触发了四次挥手的第一个阶段
        // 当 fd 发生可读事件，但是可读的数据为0，即 对端发送了 FIN
        ::shutdown(m_fd, SHUT_RDWR);//系统函数
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-17 00:02:12
     * @Description: 设置连接类型
     * @param {TcpConnectionType} type
     * @return {*}
     */
    void TcpConnection::setConnectionType(TcpConnectionType type)
    {
        m_connection_type = type;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-16 23:06:09
     * @Description: 监听可写事件
     * @return {*}
     */    
    void TcpConnection::listenWrite()
    {

        m_fd_event->listen(FdEvent::OUT_EVENT, std::bind(&TcpConnection::onWrite, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-16 23:07:21
     * @Description: 监听可读事件
     * @return {*}
     */
    void TcpConnection::listenRead()
    {

        m_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpConnection::onRead, this));
        m_event_loop->addEpollEvent(m_fd_event);
    }

    void TcpConnection::pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_write_dones.push_back(std::make_pair(message, done));
    }

    void TcpConnection::pushReadMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done)
    {
        m_read_dones.insert(std::make_pair(msg_id, done));
    }

/*----------------------------------------------------------------------------------------------------------------*/
    /**
     * @Author: kevin
     * @Date: 2023-09-06 14:36:48
     * @Description: 获取参数
     * @return {*}
     */    
    //获取本地地址信息结构体
    NetAddr::s_ptr TcpConnection::getLocalAddr()
    {
        return m_local_addr;
    }
    //获取对端地址信息结构体
    NetAddr::s_ptr TcpConnection::getPeerAddr()
    {
        return m_peer_addr;
    }
    //获取通信fd
    int TcpConnection::getFd()
    {
        return m_fd;
    }

}