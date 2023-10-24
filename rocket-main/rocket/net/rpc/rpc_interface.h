/*
 * @Description: rpc接口
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:26
 */

/*-----------为什么要用 enable_shared_from_this？

需要在类对象的内部中获得一个指向当前对象的shared_ptr对象。
如果在一个程序中，对象内存的生命周期全部由智能指针来管理。
在这种情况下，要在一个类的成员函数中，对外部返回 this 指针就成了一个很棘手的问题。

---------------什么时候用？

当一个类被共享智能指针 share_ptr 管理，且在类的成员函数里需要把当前类对象作为参数传给其他函数时，这时就需要传递一个指向自身的 share_ptr。
*/
#ifndef ROCKET_NET_RPC_RPC_INTERFACE_H
#define ROCKET_NET_RPC_RPC_INTERFACE_H

#include <memory>
#include <google/protobuf/message.h>
#include "rocket/net/rpc/rpc_controller.h"

namespace rocket
{

    class RpcClosure;
    //从类对象内部获取智能指针来指向this;
    class RpcInterface : public std::enable_shared_from_this<RpcInterface>
    {
    public:
        //RPC接口{请求，响应，回调函数，控制器}
        RpcInterface(const google::protobuf::Message *req, google::protobuf::Message *rsp, RpcClosure *done, RpcController *controller);
        
        //虚析构；
        virtual ~RpcInterface();

        //给客户端回应
        void reply();

        //释放资源
        void destroy();

        //分配一个由该接口处理的闭包对象。
        std::shared_ptr<RpcClosure> newRpcClosure(std::function<void()> &cb);

        //内核忙处理方法
        virtual void run() = 0;

        //为响应的信息设置错误码和错误信息
        virtual void setError(int code, const std::string &err_info) = 0;

    protected:
        //请求
        const google::protobuf::Message *m_req_base{NULL};
        //响应
        google::protobuf::Message *m_rsp_base{NULL};
        //闭包
        RpcClosure *m_done{NULL}; // callback
        //控制器
        RpcController *m_controller{NULL};
    };

}

#endif