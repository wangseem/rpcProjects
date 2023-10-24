
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:23
 */
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "rocket/net/rpc/rpc_dispatcher.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/common/log.h"
#include "rocket/common/error_code.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/net/rpc/rpc_closure.h"
#include "rocket/net/tcp/net_addr.h"
#include "rocket/net/tcp/tcp_connection.h"
#include "rocket/common/run_time.h"

namespace rocket
{

#define DELETE_RESOURCE(XX) \
    if (XX != NULL)         \
    {                       \
        delete XX;          \
        XX = NULL;          \
    }

    static RpcDispatcher *g_rpc_dispatcher = NULL;
//单例模式
    RpcDispatcher *RpcDispatcher::GetRpcDispatcher()
    {
        if (g_rpc_dispatcher != NULL)
        {
            return g_rpc_dispatcher;
        }
        g_rpc_dispatcher = new RpcDispatcher;
        return g_rpc_dispatcher;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-25 09:54:22
     * @Description: 分发器
     * @param {s_ptr} request
     * @param {s_ptr} response
     * @param {TcpConnection} *connection
     * @return {*}
     */
    void RpcDispatcher::dispatch(AbstractProtocol::s_ptr request, AbstractProtocol::s_ptr response, TcpConnection *connection)
    {

        //进行类型转换
        std::shared_ptr<TinyPBProtocol> req_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(request);
        std::shared_ptr<TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<TinyPBProtocol>(response);

        //获取服务名称
        std::string method_full_name = req_protocol->m_method_name;
        std::string service_name;
        std::string method_name;

        //获取服务的id和方法名
        rsp_protocol->m_msg_id = req_protocol->m_msg_id;
        rsp_protocol->m_method_name = req_protocol->m_method_name;

        //解析服务名和方法名
        if (!parseServiceFullName(method_full_name, service_name, method_name))
        {
            setTinyPBError(rsp_protocol, ERROR_PARSE_SERVICE_NAME, "parse service name error");
            return;
        }
        //查看有没有服务方法
        auto it = m_service_map.find(service_name);
        if (it == m_service_map.end())
        {
            ERRORLOG("%s | sericve neame[%s] not found", req_protocol->m_msg_id.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "service not found");
            return;
        }
        
        service_s_ptr service = (*it).second;


        // Look up a MethodDescriptor by name.
        const google::protobuf::MethodDescriptor *method = service->GetDescriptor()->FindMethodByName(method_name);
        //没有方法
        if (method == NULL)
        {
            ERRORLOG("%s | method neame[%s] not found in service[%s]", req_protocol->m_msg_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_SERVICE_NOT_FOUND, "method not found");
            return;
        }
        //如果有就反序列化
        google::protobuf::Message *req_msg = service->GetRequestPrototype(method).New();

        // 反序列化，将 pb_data 反序列化为 req_msg
        if (!req_msg->ParseFromString(req_protocol->m_pb_data))
        {
            ERRORLOG("%s | deserilize error", req_protocol->m_msg_id.c_str(), method_name.c_str(), service_name.c_str());
            setTinyPBError(rsp_protocol, ERROR_FAILED_DESERIALIZE, "deserilize error");
            DELETE_RESOURCE(req_msg);
            return;
        }

        INFOLOG("%s | get rpc request[%s]", req_protocol->m_msg_id.c_str(), req_msg->ShortDebugString().c_str());

        //
        google::protobuf::Message *rsp_msg = service->GetResponsePrototype(method).New();

        //rpccontroller
        RpcController *rpc_controller = new RpcController();
        rpc_controller->SetLocalAddr(connection->getLocalAddr());//本地地址
        rpc_controller->SetPeerAddr(connection->getPeerAddr());//远端地址
        rpc_controller->SetMsgId(req_protocol->m_msg_id);//msg_id;

        RunTime::GetRunTime()->m_msgid = req_protocol->m_msg_id;
        RunTime::GetRunTime()->m_method_name = method_name;
        
        //mutable也是为了突破const的限制而设置的。被mutable修饰的变量，将永远处于可变的状态，即使在一个const函数中//回调函数
        //lambda
        RpcClosure *closure = new RpcClosure(nullptr, [req_msg, rsp_msg, req_protocol, rsp_protocol, connection, rpc_controller, this]() mutable 
        {
            if (!rsp_msg->SerializeToString(&(rsp_protocol->m_pb_data)))
            {
                ERRORLOG("%s | serilize error, origin message [%s]", req_protocol->m_msg_id.c_str(), rsp_msg->ShortDebugString().c_str());
                setTinyPBError(rsp_protocol, ERROR_FAILED_SERIALIZE, "serilize error");
            }
            else
            {
                rsp_protocol->m_err_code = 0;
                rsp_protocol->m_err_info = "";
                INFOLOG("%s | dispatch success, requesut[%s], response[%s]", req_protocol->m_msg_id.c_str(), req_msg->ShortDebugString().c_str(), rsp_msg->ShortDebugString().c_str());
            }

            std::vector<AbstractProtocol::s_ptr> replay_messages;
            replay_messages.emplace_back(rsp_protocol);
            connection->reply(replay_messages);
        });

        service->CallMethod(method, rpc_controller, req_msg, rsp_msg, closure);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-19 16:16:32
     * @Description:解析服务的完成名字
     * @param {string} &full_name
     * @param {string} &service_name
     * @param {string} &method_name
     * @return {*}
     */
    bool RpcDispatcher::parseServiceFullName(const std::string &full_name, std::string &service_name, std::string &method_name)
    {
        if (full_name.empty())//名字是空的
        {
            ERRORLOG("full name empty");
            return false;
        }
        size_t i = full_name.find_first_of(".");//找到第一个点;vector<int>::iterator it;
        if (i == full_name.npos)
        {
            ERRORLOG("not find . in full name [%s]", full_name.c_str());
            return false;
        }
        service_name = full_name.substr(0, i);
        method_name = full_name.substr(i + 1, full_name.length() - i - 1);

        INFOLOG("parse sericve_name[%s] and method_name[%s] from full name [%s]", service_name.c_str(), method_name.c_str(), full_name.c_str());

        return true;
    }

    void RpcDispatcher::registerService(service_s_ptr service)
    {
        std::string service_name = service->GetDescriptor()->full_name();
        m_service_map[service_name] = service;
    }

    void RpcDispatcher::setTinyPBError(std::shared_ptr<TinyPBProtocol> msg, int32_t err_code, const std::string err_info)
    {
        msg->m_err_code = err_code;
        msg->m_err_info = err_info;
        msg->m_err_info_len = err_info.length();
    }

}