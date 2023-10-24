/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-19 16:21:32
 */
/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-18 16:29:07
 */
/*
 * @Description: protocol协议的一些一些相关内容
 * @Author: kevin
 * @Date: 2023-08-29 16:01:41
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-09-18 15:45:10
 */
#ifndef ROCKET_NET_CODER_TINYPB_PROTOCOL_H
#define ROCKET_NET_CODER_TINYPB_PROTOCOL_H

#include <string>
#include "rocket/net/coder/abstract_protocol.h"

namespace rocket
{

    //协议格式
    //1字节 int8_t —— char
    //2字节 int16_t —— short
    //4字节 int32_t —— int
    //8字节 int64_t —— long long
    //这是一个结构体
    // {开始符：整包长度：MSG_ID 长度:MSG_ID：方法名长度：方法名：错误码：错误信息长度：错误信息：protobuf序列化数据：校验码：结束符 }
    struct TinyPBProtocol : public AbstractProtocol
    {
    public:
        TinyPBProtocol() {}//协议构造函数
        ~TinyPBProtocol() {}//协议析构函数

    public:
        static char PB_START;//数据包开始标识符
        static char PB_END;//数据包结束标识符

    public:
        int32_t m_pk_len{0};//数据包长度
        int32_t m_msg_id_len{0};//msgID长度
        // msg_id 继承父类

        int32_t m_method_name_len{0};//方法名长度
        std::string m_method_name;//方法名
        int32_t m_err_code{0};//错误码
        int32_t m_err_info_len{0};//错误信息长度
        std::string m_err_info;//错误信息
        std::string m_pb_data;//序列化数据
        int32_t m_check_sum{0};//校验码

        bool parse_success{false};//解析成功标志
    };

}
//问题？
//既然用了protobuf做序列化，为什么不直接把序列化后的结果直接发送；而要在上面再自定义一些字段？
// 1 为了方便分割请求；因为protobuf后的结果是一串无意义的字节流，你无法区分哪里是开始或者结束，也就是解决类似tcp粘包的问题
// 2 为了定位：加上msg_id 等信息之后，能帮助我们匹配一次RPC的请求和响应，不会串包
// 3 错误提升： 加上错误信息，能很容易的直到RPC 失败的原因，方便问题定位；

#endif