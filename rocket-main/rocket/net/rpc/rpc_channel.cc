/*
 * @Description: 客户端跟服务端进行通讯，就是把connect write read 封装起来
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:06:29
 */
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include "rocket/net/rpc/rpc_channel.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/net/coder/tinypb_protocol.h"
#include "rocket/net/tcp/tcp_client.h"
#include "rocket/common/log.h"
#include "rocket/common/msg_id_util.h"
#include "rocket/common/error_code.h"
#include "rocket/common/run_time.h"
#include "rocket/net/timer_event.h"

namespace rocket
{
    /**
     * @Author: kevin
     * @Date: 2023-09-25 14:41:39
     * @Description:构造函数初始化 对端地址
     * @return {*}
     */
    RpcChannel::RpcChannel(NetAddr::s_ptr peer_addr) : m_peer_addr(peer_addr) // 对端地址
    {
        INFOLOG("RpcChannel");
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-25 14:42:14
     * @Description: 析构函数
     * @return {*}
     */
    RpcChannel::~RpcChannel()
    {
        INFOLOG("~RpcChannel");
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-25 14:42:28
     * @Description: 回调函数
     * @return {*}
     */
    void RpcChannel::callBack()
    {
        RpcController *my_controller = dynamic_cast<RpcController *>(getController());
        // 如果rpc调用完成
        if (my_controller->Finished())
        {
            return;
        }
        // 如果有回调闭包
        if (m_closure)
        {
            m_closure->Run(); // 执行回调闭包
            // 如果controller存在
            if (my_controller)
            {
                // 使用调用完成
                my_controller->SetFinished(true);
            }
        }
    }
    /***************************************************************************************/
    /**
     * @Author: kevin
     * @Date: 2023-09-25 14:47:49
     * @Description: 调用RPC方法
     * @return {*}
     */
    void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                                google::protobuf::RpcController *controller, const google::protobuf::Message *request,
                                google::protobuf::Message *response, google::protobuf::Closure *done)
    {
        std::shared_ptr<rocket::TinyPBProtocol> req_protocol = std::make_shared<rocket::TinyPBProtocol>(); // 请求

        RpcController *my_controller = dynamic_cast<RpcController *>(controller); // 控制器父类转化为子类
        // 如果控制器和请求和响应一个为空，报错
        if (my_controller == NULL || request == NULL || response == NULL)
        {
            ERRORLOG("failed callmethod, RpcController convert error");
            my_controller->SetError(ERROR_RPC_CHANNEL_INIT, "controller or request or response NULL");
            callBack();
            return;
        }
        // 如果对端地址
        if (m_peer_addr == nullptr)
        {
            ERRORLOG("failed get peer addr");
            my_controller->SetError(ERROR_RPC_PEER_ADDR, "peer addr nullptr");
            callBack();
            return;
        }
        // 创建TCPClient
        m_client = std::make_shared<TcpClient>(m_peer_addr);

        if (my_controller->GetMsgId().empty()) // 如果msg——id 是空的
        {
            // 先从 runtime 里面取, 取不到再生成一个
            // 这样的目的是为了实现 msg_id 的透传，假设服务 A 调用了 B，那么同一个 msgid 可以在服务 A 和 B 之间串起来，方便日志追踪
            std::string msg_id = RunTime::GetRunTime()->m_msgid; // 获取运行时间

            if (!msg_id.empty())
            {
                req_protocol->m_msg_id = msg_id;
                my_controller->SetMsgId(msg_id);
            }
            else
            {
                req_protocol->m_msg_id = MsgIDUtil::GenMsgID();
                my_controller->SetMsgId(req_protocol->m_msg_id);
            }
        }
        else
        {
            // 如果 controller 指定了 msg no, 直接使用
            req_protocol->m_msg_id = my_controller->GetMsgId();
        }
        // 调用方法
        req_protocol->m_method_name = method->full_name();
        INFOLOG("%s | call method name [%s]", req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str());

        //调用方法之前先看一下参数中那些个变量是否保存成功
        if (!m_is_init)
        {
            std::string err_info = "RpcChannel not call init()";
            my_controller->SetError(ERROR_RPC_CHANNEL_INIT, err_info);
            ERRORLOG("%s | %s, RpcChannel not init ", req_protocol->m_msg_id.c_str(), err_info.c_str());
            callBack();
            return;
        }

        // requeset 的序列化然后把序列化后的request 存到pb_data里
        if (!request->SerializeToString(&(req_protocol->m_pb_data)))
        {
            //如果没有序列化成功就打印许错误日志
            std::string err_info = "failde to serialize";
            my_controller->SetError(ERROR_FAILED_SERIALIZE, err_info);
            ERRORLOG("%s | %s, origin requeset [%s] ", req_protocol->m_msg_id.c_str(), err_info.c_str(), request->ShortDebugString().c_str());
            callBack();
            return;
        }

        s_ptr channel = shared_from_this();//channel指针；

        //回调函数 makeshared<TimerEvent>   TimerEvent(int interval时间间隔, bool is_repeated重复, std::function<void()> cb回调函数);
        TimerEvent::s_ptr timer_event = std::make_shared<TimerEvent>
        (my_controller->GetTimeout(), false, [my_controller, channel]() mutable
        {
            INFOLOG("%s | call rpc timeout arrive", my_controller->GetMsgId().c_str());
            if (my_controller->Finished()) {
                 channel.reset();
                return;
            }

            my_controller->StartCancel();
            my_controller->SetError(ERROR_RPC_CALL_TIMEOUT, "rpc call timeout " + std::to_string(my_controller->GetTimeout()));

            channel->callBack();
            channel.reset(); 
        });

        //--------------------------------tcp Client—connect;-----------------------------------------//
        //tcp_client添加定时任务
        m_client->addTimerEvent(timer_event);

        //tcp——client建立连接 异步的进行 conenct 三层的lambda表达式
        //lambda---------------- // 如果connect 成功，回调函数会被执行
        m_client->connect([req_protocol, this]() mutable
        {
            //获取当前的RPC控制器
            RpcController *my_controller = dynamic_cast<RpcController *>(getController());

            //如果client 错误代码！=0 说明错误
            if (getTcpClient()->getConnectErrorCode() != 0)
            {
                my_controller->SetError(getTcpClient()->getConnectErrorCode(), getTcpClient()->getConnectErrorInfo());
                //打印错误日志
                ERRORLOG("%s | connect error, error coode[%d], error info[%s], peer addr[%s]",req_protocol->m_msg_id.c_str(), my_controller->GetErrorCode(),my_controller->GetErrorInfo().c_str(), getTcpClient()->getPeerAddr()->toString().c_str());
                callBack();
                return;
            }

            //否则成功打印日志
            INFOLOG("%s | connect success, peer addr[%s], local addr[%s]",req_protocol->m_msg_id.c_str(),getTcpClient()->getPeerAddr()->toString().c_str(),getTcpClient()->getLocalAddr()->toString().c_str());
            
            //lambda ------- 发送message对象（请求数据包message，回调函数）
            //做两件事 // 1. 把 message 对象写入到 Connection 的 buffer, done 也写入 2. 启动 connection 可写事件
            getTcpClient()->writeMessage(req_protocol, [req_protocol, this, my_controller](AbstractProtocol::s_ptr) mutable
            {
                //发送成功之后打印日志发送RPC请求成功
                INFOLOG("%s | send rpc request success. call method name[%s], peer addr[%s], local addr[%s]",req_protocol->m_msg_id.c_str(), req_protocol->m_method_name.c_str(),getTcpClient()->getPeerAddr()->toString().c_str(), getTcpClient()->getLocalAddr()->toString().c_str());
                //lambda
                //然后就去读取
                getTcpClient()->readMessage(req_protocol->m_msg_id, [this, my_controller](AbstractProtocol::s_ptr msg) mutable
                {

                    //创建响应message
                    std::shared_ptr<rocket::TinyPBProtocol> rsp_protocol = std::dynamic_pointer_cast<rocket::TinyPBProtocol>(msg);
                    //读成功之后我们打印日志
                    INFOLOG("%s | success get rpc response, call method name[%s], peer addr[%s], local addr[%s]",rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),getTcpClient()->getPeerAddr()->toString().c_str(), getTcpClient()->getLocalAddr()->toString().c_str());
                    //直接反序列化
                    if (!(getResponse()->ParseFromString(rsp_protocol->m_pb_data)))
                    {
                        //如果失败了那么就直接给爷打印错误日志，并设置错误代码
                        ERRORLOG("%s | serialize error", rsp_protocol->m_msg_id.c_str());
                        my_controller->SetError(ERROR_FAILED_SERIALIZE, "serialize error");
                        callBack();
                        return;
                    }
                    //如果错误码是0，那么说明这个响应是失败的：=，也就是调用rpc方法失败
                    if (rsp_protocol->m_err_code != 0)
                    {
                        //打印错误日志
                        ERRORLOG("%s | call rpc methood[%s] failed, error code[%d], error info[%s]",rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),rsp_protocol->m_err_code, rsp_protocol->m_err_info.c_str());
                        //设置错误码错误信息
                        my_controller->SetError(rsp_protocol->m_err_code, rsp_protocol->m_err_info);
                        callBack();
                        return;
                    }

                    // 如果都没有问题就去调用这个回调函数
                    INFOLOG("%s | call rpc success, call method name[%s], peer addr[%s], local addr[%s]",rsp_protocol->m_msg_id.c_str(), rsp_protocol->m_method_name.c_str(),getTcpClient()->getPeerAddr()->toString().c_str(), getTcpClient()->getLocalAddr()->toString().c_str());
                    callBack();
                });
            }); 
        });
    }
 
    /**
     * @Author: kevin
     * @Date: 2023-09-25 20:59:59
     * @Description: 其实就是保存callmethod里面的一些变量，做一下拷贝；
     * @param {controller_s_ptr} controller
     * @param {message_s_ptr} req
     * @param {message_s_ptr} res
     * @param {closure_s_ptr} done
     * @return {*}
     */    
    void RpcChannel::Init(controller_s_ptr controller, message_s_ptr req, message_s_ptr res, closure_s_ptr done)
    {
        if (m_is_init)
        {
            return;
        }
        m_controller = controller;
        m_request = req;
        m_response = res;
        m_closure = done;
        m_is_init = true;
    }

    google::protobuf::RpcController *RpcChannel::getController()
    {
        return m_controller.get();
    }

    google::protobuf::Message *RpcChannel::getRequest()
    {
        return m_request.get();
    }

    google::protobuf::Message *RpcChannel::getResponse()
    {
        return m_response.get();
    }

    google::protobuf::Closure *RpcChannel::getClosure()
    {
        return m_closure.get();
    }

    TcpClient *RpcChannel::getTcpClient()
    {
        return m_client.get();
    }
    //
    NetAddr::s_ptr RpcChannel::FindAddr(const std::string &str)
    {
        if (IPNetAddr::CheckValid(str)) // 查看这个str是不是有效
        {
            return std::make_shared<IPNetAddr>(str);
        }
        else
        {
            auto it = Config::GetGlobalConfig()->m_rpc_stubs.find(str);
            if (it != Config::GetGlobalConfig()->m_rpc_stubs.end())
            {
                INFOLOG("find addr [%s] in global config of str[%s]", (*it).second.addr->toString().c_str(), str.c_str());
                return (*it).second.addr;
            }
            else
            {
                INFOLOG("can not find addr in global config of str[%s]", str.c_str());
                return nullptr;
            }
        }
    }

}