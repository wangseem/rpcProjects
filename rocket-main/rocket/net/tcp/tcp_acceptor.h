/*
 * @Description: tcp accept函数你懂吗
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:13:55
 */
#ifndef ROCKET_NET_TCP_TCP_ACCEPTOR_H
#define ROCKET_NET_TCP_TCP_ACCEPTOR_H

#include <memory>
#include "rocket/net/tcp/net_addr.h"

namespace rocket
{

    class TcpAcceptor
    {
    public:
        //你有没有发现他都会有一个智能指针
        //我们也试试
        //typedef std::sharef_ptr<TcpAcceptor> s_ptr; 我们每次用的时候都使用域作用符
        typedef std::shared_ptr<TcpAcceptor> s_ptr;

        //构造函数，传入的是一个网络地址 
        TcpAcceptor(NetAddr::s_ptr local_addr);

        // 析构
        ~TcpAcceptor();

        //accept函数我目测啊是封装了socket的accept(m_listenfd, reinterpret_cast<sockaddr *>(&client_addr), &clien_addr_len);
        std::pair<int, NetAddr::s_ptr> accept();

        int getListenFd();

    private:
        NetAddr::s_ptr m_local_addr; // 服务端监听的地址，addr -> ip:port

        int m_family{-1};

        int m_listenfd{-1}; // 监听套接字
    };

}

#endif