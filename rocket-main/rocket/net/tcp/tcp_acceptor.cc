#include <assert.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_acceptor.h"

namespace rocket
{

    TcpAcceptor::TcpAcceptor(NetAddr::s_ptr local_addr) : m_local_addr(local_addr)
    {
        if (!local_addr->checkValid())//查看传入参数是否有效//地址是否空，端口是否合理，
        {
            ERRORLOG("invalid local addr %s", local_addr->toString().c_str());
            exit(0);
        }

        //获取ip的协议族
        m_family = m_local_addr->getFamily();

        //获取套接字
        m_listenfd = socket(m_family, SOCK_STREAM, 0);
        if (m_listenfd < 0)
        {
            ERRORLOG("invalid listenfd %d", m_listenfd);
            exit(0);
        }

        //设置socket选项
        int val = 1;
        if (setsockopt(m_listenfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) != 0)
        {
            ERRORLOG("setsockopt REUSEADDR error, errno=%d, error=%s", errno, strerror(errno));
        }
        //绑定socket
        socklen_t len = m_local_addr->getSockLen();
        if (bind(m_listenfd, m_local_addr->getSockAddr(), len) != 0)
        {
            ERRORLOG("bind error, errno=%d, error=%s", errno, strerror(errno));
            exit(0);
        }

        //监听listen成功返回0，错位返回 -1;
        if (listen(m_listenfd, 1000) != 0)
        {
            ERRORLOG("listen error, errno=%d, error=%s", errno, strerror(errno));
            exit(0);
        }
    }

    TcpAcceptor::~TcpAcceptor()
    {
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-06 13:33:16
     * @Description: //获取监听的fd
     * @return {*}
     */
    int TcpAcceptor::getListenFd()
    {
        return m_listenfd;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-06 13:26:55
     * @Description: 封装了accept函数
     * @return {*}对端地址和fd
     */
    std::pair<int, NetAddr::s_ptr> TcpAcceptor::accept()
    {
        if (m_family == AF_INET)//首先判断一下协议族;
        {
            sockaddr_in client_addr;
            memset(&client_addr, 0, sizeof(client_addr));
            //bzero(&client_addr, sizeof(client_addr));
            socklen_t clien_addr_len = sizeof(clien_addr_len);

            int client_fd = ::accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &clien_addr_len);
            if (client_fd < 0)
            { 
                ERRORLOG("accept error, errno=%d, error=%s", errno, strerror(errno));
            }
            IPNetAddr::s_ptr peer_addr = std::make_shared<IPNetAddr>(client_addr);
            INFOLOG("A client have accpeted succ, peer addr [%s]", peer_addr->toString().c_str());
            //这个peer 节点是什么意思啊，兄弟

            return std::make_pair(client_fd, peer_addr);
        }
        else
        {
            // ...
            return std::make_pair(-1, nullptr);
        }
    }

}