/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:15:03
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:15:00
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:14:58
 */
#include <string.h>
#include "rocket/common/log.h"
#include "rocket/net/tcp/net_addr.h"

namespace rocket
{

    

    IPNetAddr::IPNetAddr(const std::string &ip, uint16_t port) : m_ip(ip), m_port(port)
    {
        memset(&m_addr, 0, sizeof(m_addr));

        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        m_addr.sin_port = htons(m_port);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:40:58
     * @Description: 构造函数
     * @param {string} &addr  如果是string 类型就要在构造时自己解析
     * @return {*}
     */
    IPNetAddr::IPNetAddr(const std::string &addr)
    {
        size_t i = addr.find_first_of(":");
        if (i == addr.npos)
        {
            ERRORLOG("invalid ipv4 addr %s", addr.c_str());
            return;
        }
        m_ip = addr.substr(0, i);
        m_port = std::atoi(addr.substr(i + 1, addr.size() - i - 1).c_str());

        memset(&m_addr, 0, sizeof(m_addr));
        m_addr.sin_family = AF_INET;
        m_addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
        m_addr.sin_port = htons(m_port);
    }

    //有参构造
    IPNetAddr::IPNetAddr(sockaddr_in addr) : m_addr(addr)
    {
        m_ip = std::string(inet_ntoa(m_addr.sin_addr));
        m_port = ntohs(m_addr.sin_port);
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:38:02
     * @Description: 获取socket地址
     * @return {*}
     */
    sockaddr *IPNetAddr::getSockAddr()
    {
        return reinterpret_cast<sockaddr *>(&m_addr);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:39:10
     * @Description: 获取sock长度
     * @return {*}
     */    
    socklen_t IPNetAddr::getSockLen()
    {
        return sizeof(m_addr);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:39:10
     * @Description: 获取sock长度
     * @return {*}
     */    
    int IPNetAddr::getFamily()
    {
        return AF_INET;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:39:50
     * @Description:返回ip 和端口转换为string
     * @return {*}
     */    
    std::string IPNetAddr::toString()
    {
        std::string re;
        re = m_ip + ":" + std::to_string(m_port);
        return re;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:40:18
     * @Description: 检查是否有效
     * @return {*}
     */    
    bool IPNetAddr::checkValid()
    {
        if (m_ip.empty())
        {
            return false;
        }

        if (m_port < 0 || m_port > 65536)
        {
            return false;
        }

        if (inet_addr(m_ip.c_str()) == INADDR_NONE)
        {
            return false;
        }
        return true;
    }
    
    /**
     * @Author: kevin
     * @Date: 2023-09-04 21:40:18
     * @Description: 检查是否有效
     * @return {*}
     */    
    bool IPNetAddr::CheckValid(const std::string &addr)
    {
        size_t i = addr.find_first_of(":");
        if (i == addr.npos)
        {
            return false;
        }
        std::string ip = addr.substr(0, i);
        std::string port = addr.substr(i + 1, addr.size() - i - 1);
        if (ip.empty() || port.empty())
        {
            return false;
        }

        int iport = std::atoi(port.c_str());
        if (iport <= 0 || iport > 65536)
        {
            return false;
        }

        return true;
    }
}