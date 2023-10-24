/*
 * @Description: 执行一个回调，closure是闭包的意思，
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-25 23:28:36
 */
#ifndef ROCKET_NET_RPC_RPC_CLOSURE_H
#define ROCKET_NET_RPC_RPC_CLOSURE_H

#include <google/protobuf/stubs/callback.h>
#include <functional>
#include <memory>
#include "rocket/common/run_time.h"
#include "rocket/common/log.h"
#include "rocket/common/exception.h"
#include "rocket/net/rpc/rpc_interface.h"

namespace rocket
{
    class RpcClosure : public google::protobuf::Closure//这个类里面有一个run函数需要重写
    {
    public:
        typedef std::shared_ptr<RpcInterface> it_s_ptr;  //rpc接口智能指针
        //有参构造
        RpcClosure(it_s_ptr interface, std::function<void()> cb) : m_rpc_interface(interface), m_cb(cb)
        {
            INFOLOG("RpcClosure");
        }

        ~RpcClosure()
        {
            INFOLOG("~RpcClosure");
        }


        /**
         * @Author: kevin
         * @Date: 2023-09-25 21:23:04
         * @Description: 函数的主要目的是干什么？//怎么干啥都调用它
         * @return {*}
         */
        void Run() override
        {

            // 更新 runtime 的 RpcInterFace, 这里在执行 cb 的时候，都会以 RpcInterface 找到对应的接口，实现打印 app 日志等
            if (!m_rpc_interface)
            {
                RunTime::GetRunTime()->m_rpc_interface = m_rpc_interface.get();
            }

            try
            {
                if (m_cb != nullptr)
                {
                    m_cb();
                }
                if (m_rpc_interface)
                {
                    m_rpc_interface.reset();//interface 的引用数重置
                }
            }
            catch (RocketException &e)
            {
                ERRORLOG("RocketException exception[%s], deal handle", e.what());
                e.handle();
                if (m_rpc_interface)
                {
                    m_rpc_interface->setError(e.errorCode(), e.errorInfo());
                    m_rpc_interface.reset();
                }
            }
            catch (std::exception &e)
            {
                ERRORLOG("std::exception[%s]", e.what());
                if (m_rpc_interface)
                {
                    m_rpc_interface->setError(-1, "unkonwn std::exception");
                    m_rpc_interface.reset();
                }
            }
            catch (...)
            {
                ERRORLOG("Unkonwn exception");
                if (m_rpc_interface)
                {
                    m_rpc_interface->setError(-1, "unkonwn exception");
                    m_rpc_interface.reset();
                }
            }
        }

    private:
        it_s_ptr m_rpc_interface{nullptr}; //
        std::function<void()> m_cb{nullptr};//回调函数
    };

}
#endif