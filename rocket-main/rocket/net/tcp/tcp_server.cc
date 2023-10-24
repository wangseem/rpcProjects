/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:13:07
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:13:04
 */
#include "rocket/net/tcp/tcp_server.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/common/log.h"
#include "rocket/common/config.h"

namespace rocket
{

    TcpServer::TcpServer(NetAddr::s_ptr local_addr) : m_local_addr(local_addr)
    {

        init();
        INFOLOG("rocket TcpServer listen sucess on [%s]", m_local_addr->toString().c_str());
    }

    TcpServer::~TcpServer()
    {
        if (m_main_event_loop)
        {
            delete m_main_event_loop;
            m_main_event_loop = NULL;
        }
        if (m_io_thread_group)
        {
            delete m_io_thread_group;
            m_io_thread_group = NULL;
        }
        if (m_listen_fd_event)
        {
            delete m_listen_fd_event;
            m_listen_fd_event = NULL;
        }
    }

    void TcpServer::init()
    {

        m_acceptor = std::make_shared<TcpAcceptor>(m_local_addr);//里面的m_local_addr是TcpAcceptor构造函数的参数

        m_main_event_loop = EventLoop::GetCurrentEventLoop();//获取主线程的线程id
        m_io_thread_group = new IOThreadGroup(Config::GetGlobalConfig()->m_io_threads);//获取io线程在哦我们的配置文件里面有我们自己定义的io线程数

        m_listen_fd_event = new FdEvent(m_acceptor->getListenFd());//获取监听的socket
        m_listen_fd_event->listen(FdEvent::IN_EVENT, std::bind(&TcpServer::onAccept, this));//fd_event可读的时候就调用onacceptor回调函数

        m_main_event_loop->addEpollEvent(m_listen_fd_event);

        m_clear_client_timer_event = std::make_shared<TimerEvent>(5000, true, std::bind(&TcpServer::ClearClientTimerFunc, this));
        m_main_event_loop->addTimerEvent(m_clear_client_timer_event);
    }

    void TcpServer::onAccept()
    {
        auto re = m_acceptor->accept();
        int client_fd = re.first;
        NetAddr::s_ptr peer_addr = re.second;

        m_client_counts++;//客户端数量增加

        // 把 cleintfd 添加到任意 IO 线程里面
        IOThread *io_thread = m_io_thread_group->getIOThread();
        
        TcpConnection::s_ptr connetion = std::make_shared<TcpConnection>(io_thread->getEventLoop(), client_fd, 128, peer_addr, m_local_addr);
        connetion->setState(Connected);

        m_client.insert(connetion);

        INFOLOG("TcpServer succ get client, fd=%d", client_fd);
    }

    void TcpServer::start()
    {
        m_io_thread_group->start();//开启我们的io子线程
        m_main_event_loop->loop();//在我们的loop循环阻塞
    }

    void TcpServer::ClearClientTimerFunc()
    {
        auto it = m_client.begin();
        for (it = m_client.begin(); it != m_client.end();)
        {
            // TcpConnection::ptr s_conn = i.second;
            // DebugLog << "state = " << s_conn->getState();
            if ((*it) != nullptr && (*it).use_count() > 0 && (*it)->getState() == Closed)
            {
                // need to delete TcpConnection
                DEBUGLOG("TcpConection [fd:%d] will delete, state=%d", (*it)->getFd(), (*it)->getState());
                it = m_client.erase(it);
            }
            else
            {
                it++;
            }
        }
    }

}