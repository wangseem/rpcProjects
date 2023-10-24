/*
 * @Description: RPC调度分发器；将RPC请求传给分发器然后分发器去相应RPC请求；
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 16:45:58
 */
#ifndef ROCKET_NET_RPC_RPC_DISPATCHER_H
#define ROCKET_NET_RPC_RPC_DISPATCHER_H

#include <map>
#include <memory>
#include <google/protobuf/service.h>

#include "rocket/net/coder/abstract_protocol.h"
#include "rocket/net/coder/tinypb_protocol.h"

namespace rocket
{

    class TcpConnection;//前置依赖，防止循环依赖

    class RpcDispatcher
    {

    public:
        static RpcDispatcher *GetRpcDispatcher();

    public:

        typedef std::shared_ptr<google::protobuf::Service> service_s_ptr;

        //调度分发器
        void dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection *connection);

        //注册RPC服务方法
        void registerService(service_s_ptr service);

        //设置错误信息和错误信息码
        void setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info);

    private:
        //解析服务的名字
        bool parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name);

    private:
        std::map<std::string, service_s_ptr> m_service_map;//服务注册
    };

}

#endif