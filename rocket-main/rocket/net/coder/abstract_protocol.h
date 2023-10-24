/*
 * @Description: 通用抽象协议，方便实现rpc调用
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-17 00:44:25
 */
#ifndef ROCKET_NET_ABSTRACT_PROTOCOL_H
#define ROCKET_NET_ABSTRACT_PROTOCOL_H

#include <memory>

namespace rocket
{

  struct AbstractProtocol : public std::enable_shared_from_this<AbstractProtocol>
  {
  public:
    typedef std::shared_ptr<AbstractProtocol> s_ptr;

    virtual ~AbstractProtocol() {}

  public:
    std::string m_msg_id; // 请求号，唯一标识一个请求或者响应
  };

}

#endif
