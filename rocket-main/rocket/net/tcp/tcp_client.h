/*
 * @Description: tcp 客户端
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:33
 */
#ifndef ROCKET_NET_TCP_TCP_CLIENT_H
#define ROCKET_NET_TCP_TCP_CLIENT_H
#include <memory>
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/eventloop.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/net/coder/abstract_protocol.h"
#include "rocket/net/timer_event.h"

namespace rocket
{

    class TcpClient
    {
    public:
        typedef std::shared_ptr<TcpClient> s_ptr;

        TcpClient(NetAddr::s_ptr peer_addr);

        ~TcpClient();

        // 异步的进行 conenct
        // 如果 connect 完成，done 回调函数会被执行
        void connect(std::function<void()> done);

        // 异步的发送 message//广义的字符串，可以是字符串，也可以是rpcrpc协议
        // 如果发送 message 成功，会调用 done 函数， 函数的入参就是 message 对象
        void writeMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        // 异步的读取 message
        // 如果读取 message 成功，会调用 done 函数， 函数的入参就是 message 对象
        void readMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done);
        //结束eventloop的方法
        void stop();

        //获取错误码
        int getConnectErrorCode();

        //获取错误信息
        std::string getConnectErrorInfo();

        //获取地址
        NetAddr::s_ptr getPeerAddr();
        NetAddr::s_ptr getLocalAddr();

        //获取fd所绑定的四元组 可以是get
        void initLocalAddr();

        void addTimerEvent(TimerEvent::s_ptr timer_event);

    private:
    /****************************地址模块*********************************/
        NetAddr::s_ptr m_peer_addr;     //智能指针
        NetAddr::s_ptr m_local_addr;    //智能指针

        EventLoop *m_event_loop{NULL};  //事件循环

        int m_fd{-1};                   //文件描述符
        FdEvent *m_fd_event{NULL};      //epoll_event

        TcpConnection::s_ptr m_connection;      // tcp_connection

        int m_connect_error_code{0};            // 连接失败的错误码
        std::string m_connect_error_info;       //连接失败的错误信息
    };
}

#endif