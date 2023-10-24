/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-25 23:28:30
 */
#include "rocket/common/log.h"
#include "rocket/net/rpc/rpc_closure.h"
#include "rocket/net/rpc/rpc_closure.h"
#include "rocket/net/rpc/rpc_controller.h"
#include "rocket/net/rpc/rpc_interface.h"

namespace rocket
{
    /**
     * @Author: kevin
     * @Date: 2023-09-25 21:37:06
     * @Description: 构造函数，目的就是初始化这些b
     * @param {Message} *req
     * @param {Message} *rsp
     * @param {RpcClosure} *done
     * @param {RpcController} *controller
     * @return {*}
     */
    RpcInterface::RpcInterface(const google::protobuf::Message *req, google::protobuf::Message *rsp, RpcClosure *done, RpcController *controller)
        : m_req_base(req), m_rsp_base(rsp), m_done(done), m_controller(controller)
    {
        INFOLOG("RpcInterface");
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-25 21:37:32
     * @Description: 释放资源
     * @return {*}
     */    
    RpcInterface::~RpcInterface()
    {
        INFOLOG("~RpcInterface");
        reply();
        destroy();
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-25 21:38:15
     * @Description: // 回复给客户端
     *   // 当您希望设置响应时，您应该调用它
     *   // 这意味着此RPC方法已完成
     * @return {*}
     */
    void RpcInterface::reply()
    {
        if (m_done)
        {
            m_done->Run();
        }
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-25 21:51:28
     * @Description: //分配一个由该接口处理的闭包对象。
     * @param {function<void()>} &cb
     * @return {*}
     */
    std::shared_ptr<RpcClosure> RpcInterface::newRpcClosure(std::function<void()> &cb)
    {
        return std::make_shared<RpcClosure>(shared_from_this(), cb);
    }

    /**
     * @Author: kevin
     * @Date: 2023-09-25 21:50:59
     * @Description:释放资源 
     * @return {*}
     */
    void RpcInterface::destroy()
    {
        if (m_req_base)
        {
            delete m_req_base;
            m_req_base = NULL;
        }
        if (m_rsp_base)
        {
            delete m_rsp_base;
            m_rsp_base = NULL;
        }
        if (m_done)
        {
            delete m_done;
            m_done = NULL;
        }
        if (m_controller)
        {
            delete m_controller;
            m_controller = NULL;
        }
    }

}