/*
 * @Description: 封装网络地址
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:13:39
 */
#ifndef ROCKET_NET_TCP_NET_ADDR_H
#define ROCKET_NET_TCP_NET_ADDR_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>
#include <memory>

namespace rocket
{

    // 抽象类用作父类
    class NetAddr
    {
    public:
        typedef std::shared_ptr<NetAddr> s_ptr;

        virtual sockaddr *getSockAddr() = 0;

        virtual socklen_t getSockLen() = 0;

        virtual int getFamily() = 0;

        virtual std::string toString() = 0;

        virtual bool checkValid() = 0; // 检测合法性
    };

    // 子类
    class IPNetAddr : public NetAddr
    {

    public:
        static bool CheckValid(const std::string &addr);

    public:
        // 构造函数三个
        IPNetAddr(const std::string &ip, uint16_t port);

        IPNetAddr(const std::string &addr);

        IPNetAddr(sockaddr_in addr);

        // 重写的父类的函数
        // 获取sock地址
        sockaddr *getSockAddr();

        // 获取sock长度
        socklen_t getSockLen();

        // 获取协议族
        int getFamily();

        // 转换成字符串返回ip 和端口
        std::string toString();

        // 检查sockaddr是否有效
        bool checkValid();

    private:
        std::string m_ip;   // ip
        uint16_t m_port{0}; // 端口号
        sockaddr_in m_addr; //地址信息包含端口号和ip
        /*struct sockaddr_in
        {
            struct in_addr sin_addr;		
            unsigned char sin_zero[sizeof (struct sockaddr)
			        - __SOCKADDR_COMMON_SIZE
			        - sizeof (in_port_t)
			        - sizeof (struct in_addr)];
        };*/
    };

}

#endif
