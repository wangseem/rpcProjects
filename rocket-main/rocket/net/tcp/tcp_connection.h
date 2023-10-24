/*
 * @Description: tcp_connection连接就是去处理缓冲区数据
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-18 16:29:33
 */
#ifndef ROCKET_NET_TCP_TCP_CONNECTION_H
#define ROCKET_NET_TCP_TCP_CONNECTION_H

#include <memory>
#include <map>
#include <queue>
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_buffer.h"
#include "rocket/net/io_thread.h"
#include "rocket/net/coder/abstract_coder.h"
#include "rocket/net/rpc/rpc_dispatcher.h"

namespace rocket
{

    enum TcpState//tcp状态枚举字
    {
        NotConnected = 1,
        Connected = 2,
        HalfClosing = 3,
        Closed = 4,
    };

    enum TcpConnectionType
    {
        TcpConnectionByServer = 1, // 作为服务端使用，代表跟对端客户端的连接
        TcpConnectionByClient = 2, // 作为客户端使用，代表跟对端服务端的连接
    };

    class TcpConnection
    {
    public:
        typedef std::shared_ptr<TcpConnection> s_ptr;

    public:
        TcpConnection(EventLoop *event_loop, int fd, int buffer_size, NetAddr::s_ptr peer_addr, NetAddr::s_ptr local_addr, TcpConnectionType type = TcpConnectionByServer);

        ~TcpConnection();

        // connnection的三个和新方法
        void onRead();

        void excute();

        void onWrite();


        //设置tcp状态
        void setState(const TcpState state);

        TcpState getState();
        //清理连接
        void clear();
        //获取fd
        int getFd();

        // 服务器主动关闭连接
        void shutdown();

        //设置连接类型
        void setConnectionType(TcpConnectionType type);

        // 启动监听可写事件
        void listenWrite();

        // 启动监听可读事件
        void listenRead();

        void pushSendMessage(AbstractProtocol::s_ptr message, std::function<void(AbstractProtocol::s_ptr)> done);

        void pushReadMessage(const std::string &msg_id, std::function<void(AbstractProtocol::s_ptr)> done);

        NetAddr::s_ptr getLocalAddr();

        NetAddr::s_ptr getPeerAddr();

        void reply(std::vector<AbstractProtocol::s_ptr> &replay_messages);

    private:
        /// 我们定义的io线程其实就是去创建一个线程然后去调用在这个eventloop；
        EventLoop *m_event_loop{NULL}; // 代表持有该连接的 IO 线程//这个io线程就是我们event——loop

        NetAddr::s_ptr m_local_addr;//本地地址
        NetAddr::s_ptr m_peer_addr;//对端服务器的地址

        TcpBuffer::s_ptr m_in_buffer;  // 接收缓冲区
        TcpBuffer::s_ptr m_out_buffer; // 发送缓冲区

        FdEvent *m_fd_event{NULL};

        AbstractCoder *m_coder{NULL};

        TcpState m_state;

        int m_fd{0};

        TcpConnectionType m_connection_type{TcpConnectionByServer};

        // std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>
        std::vector<std::pair<AbstractProtocol::s_ptr, std::function<void(AbstractProtocol::s_ptr)>>> m_write_dones;//保存写入回调函数

        // key 为 msg_id
        std::map<std::string, std::function<void(AbstractProtocol::s_ptr)>> m_read_dones;//保存我们的回调函数
    };

}

#endif
