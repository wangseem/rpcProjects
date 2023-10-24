/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-21 15:07:31
 */
#ifndef ROCKER_NET_RPC_RPC_CONTROLLER_H
#define ROCKER_NET_RPC_RPC_CONTROLLER_H

#include <google/protobuf/service.h>
#include <google/protobuf/stubs/callback.h>
#include <string>

#include "rocket/net/tcp/net_addr.h"
#include "rocket/common/log.h"

namespace rocket
{

    class RpcController : public google::protobuf::RpcController
    {

    public:
        RpcController() { INFOLOG("RpcController"); }
        ~RpcController() { INFOLOG("~RpcController"); }
        ///////////////客户端调用方法
        //重新设置控制器
        //这些调用只能在客户端进行。他们的研究结果
        //服务器端未定义(可能崩溃)
        //重置RpcController到它的初始状态，以便它可以被重用
        //一个新的调用。在RPC正在进行时不得调用。
        void Reset();
        //RPC是否失败  只读
        //调用结束后，如果调用失败返回true。可能的
        //失败的原因取决于RPC实现。Failed()不能
        //在调用结束之前被调用。如果Failed()返回true，则
        //响应消息的内容未定义。
        bool Failed() const;
        //错误信息if is failed == ture 
        std::string ErrorText() const;
        //取消
        void StartCancel();

        ////////////////////////
        //设置失败
        void SetFailed(const std::string &reason);




        bool IsCanceled() const;

        void NotifyOnCancel(google::protobuf::Closure *callback);




//
        void SetError(int32_t error_code, const std::string error_info);

        int32_t GetErrorCode();

        std::string GetErrorInfo();

        void SetMsgId(const std::string &msg_id);

        std::string GetMsgId();

        void SetLocalAddr(NetAddr::s_ptr addr);

        void SetPeerAddr(NetAddr::s_ptr addr);

        NetAddr::s_ptr GetLocalAddr();

        NetAddr::s_ptr GetPeerAddr();

        void SetTimeout(int timeout);

        int GetTimeout();

        bool Finished();

        void SetFinished(bool value);

    private:
        int32_t m_error_code{0};        //错误码
        std::string m_error_info;       //错误信息
        std::string m_msg_id;           //msg_id

        bool m_is_failed{false};        //RPC失败
        bool m_is_cancled{false};       //RPC取消
        bool m_is_finished{false};      //RPC完成

        NetAddr::s_ptr m_local_addr;    //本地地址 
        NetAddr::s_ptr m_peer_addr;     //对端地址

        int m_timeout{1000}; // ms     //超时时间    
    };

}

#endif