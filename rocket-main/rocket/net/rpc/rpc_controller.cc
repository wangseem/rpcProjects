
#include "rocket/net/rpc/rpc_controller.h"

namespace rocket
{

    /**
     * @Author: kevin
     * @Date: 2023-09-19 16:44:54
     * @Description:重置控制器 
     * @return {*}
     */
    void RpcController::Reset()
    {
        m_error_code = 0;
        m_error_info = "";
        m_msg_id = "";
        m_is_failed = false;
        m_is_cancled = false;
        m_is_finished = false;
        m_local_addr = nullptr;
        m_peer_addr = nullptr;
        m_timeout = 1000; // ms
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-19 16:45:18
     * @Description: RPC是否失败只读
     * @return {*}
     */    
    bool RpcController::Failed() const
    {
        return m_is_failed;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-19 16:45:35
     * @Description:获取RPC错误信息 
     * @return {*}
     */    
    std::string RpcController::ErrorText() const
    {
        return m_error_info;
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-19 16:45:51
     * @Description: 取消RPC
     * @return {*}
     */    
    void RpcController::StartCancel()
    {
        m_is_cancled = true;
        m_is_failed = true;
        SetFinished(true);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-22 21:34:26
     * @Description: 设置我们的失败信息，就是为什么失败
     * @param {string} &reason
     * @return {*}
     */
    void RpcController::SetFailed(const std::string &reason)
    {
        m_error_info = reason;
        m_is_failed = true;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-22 21:34:49
     * @Description:是否取消 
     * @return {*}
     */
    bool RpcController::IsCanceled() const
    {
        return m_is_cancled;
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-20 11:02:54
     * @Description: 取消通知
     * @param {Closure} *callback
     * @return {*}
     */    
    void RpcController::NotifyOnCancel(google::protobuf::Closure *callback)
    {
    }
    /**
     * @Author: kevin
     * @Date: 2023-09-22 21:35:34
     * @Description:设置错误码，错误信息 
     * @param {int32_t} error_code
     * @param {string} error_info
     * @return {*}
     */    
    void RpcController::SetError(int32_t error_code, const std::string error_info)
    {
        m_error_code = error_code;
        m_error_info = error_info;
        m_is_failed = true;//rpc调用失败了
    }
    //////////////////////////////////////////////////////////////////////////////
    /**
     * @Author: kevin
     * @Date: 2023-09-20 11:03:27
     * @Description: 获取基本信息的
     * @return {*}
     */    
    int32_t RpcController::GetErrorCode()
    {
        return m_error_code;
    }

    std::string RpcController::GetErrorInfo()
    {
        return m_error_info;
    }

    void RpcController::SetMsgId(const std::string &msg_id)
    {
        m_msg_id = msg_id;
    }

    std::string RpcController::GetMsgId()
    {
        return m_msg_id;
    }

    void RpcController::SetLocalAddr(NetAddr::s_ptr addr)
    {
        m_local_addr = addr;
    }

    void RpcController::SetPeerAddr(NetAddr::s_ptr addr)
    {
        m_peer_addr = addr;
    }

    NetAddr::s_ptr RpcController::GetLocalAddr()
    {
        return m_local_addr;
    }

    NetAddr::s_ptr RpcController::GetPeerAddr()
    {
        return m_peer_addr;
    }

    void RpcController::SetTimeout(int timeout)
    {
        m_timeout = timeout;
    }

    int RpcController::GetTimeout()
    {
        return m_timeout;
    }

    bool RpcController::Finished()
    {
        return m_is_finished;
    }

    void RpcController::SetFinished(bool value)
    {
        m_is_finished = value;
    }

}