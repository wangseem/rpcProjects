/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-14 14:26:55
 */
#ifndef ROCKET_COMMON_RUN_TIME_H
#define ROCKET_COMMON_RUN_TIME_H

#include <string>

namespace rocket
{

    class RpcInterface;

    class RunTime
    {
    public:
        RpcInterface *getRpcInterface();

    public:
        static RunTime *GetRunTime();

    public:
        std::string m_msgid;
        std::string m_method_name;
        RpcInterface *m_rpc_interface{NULL};
    };

}

#endif