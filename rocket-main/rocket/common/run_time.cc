/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:16:21
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:16:12
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-16 23:16:09
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-16 23:16:06
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: 
 * @LastEditTime: 2023-09-14 14:27:02
 */

#include "rocket/common/run_time.h"

namespace rocket
{

    thread_local RunTime *t_run_time = NULL;

    //获取运行时间
    RunTime *RunTime::GetRunTime()
    {
        if (t_run_time)
        {
            return t_run_time;
        }
        t_run_time = new RunTime();
        return t_run_time;
    }

    //获取运行时间
    RpcInterface *RunTime::getRpcInterface()
    {
        return m_rpc_interface;
    }

}