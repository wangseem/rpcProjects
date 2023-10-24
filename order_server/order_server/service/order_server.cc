/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * order_server.cc
 * 2023-08-29 16:25:22
 * Generated by rocket framework rocket_generator.py
 * Do not edit !!! 就是一个接口转发
****************************************************/


#include <google/protobuf/service.h>
#include <exception>
#include <rocket/common/log.h>
#include "order_server/pb/order_server.pb.h"
#include "order_server/service/order_server.h"
#include "order_server/comm/business_exception.h"
#include "order_server/interface/query_name.h"
#include "order_server/interface/query_age.h"


#define CALL_RPC_INTERFACE(Type)                                                                                                    \
  {                                                                                                                                 \
    rocket::RpcClosure* clo = dynamic_cast<rocket::RpcClosure*>(done);                                                              \
    rocket::RpcController* con = dynamic_cast<rocket::RpcController*>(controller);                                                  \
    std::shared_ptr<Type> impl = std::make_shared<Type>(request, response, clo, con);                                               \
    rocket::RunTime::GetRunTime()->m_rpc_interface = std::dynamic_pointer_cast<rocket::RpcInterface>(impl).get();                   \
    response->set_ret_code(0);                                                                                                      \
    response->set_res_info("OK");                                                                                                   \
    try {                                                                                                                           \
      impl->run();                                                                                                                  \
    } catch (BusinessException& e) {                                                                                                \
      APPDEBUGLOG("business exception");                                                                                            \
      response->set_ret_code(e.errorCode());                                                                                        \
      response->set_res_info(e.errorInfo());                                                                                        \
    } catch (rocket::RocketException& e) {                                                                                          \
      APPDEBUGLOG("RocketException");                                                                                               \
      response->set_ret_code(-1);                                                                                                   \
      response->set_res_info("Unknown RocketException");                                                                            \
    } catch (std::exception& e) {                                                                                                   \
      APPDEBUGLOG("std::exception");                                                                                                \
      response->set_ret_code(-1);                                                                                                   \
      response->set_res_info("Unknown std::exception");                                                                             \
    } catch (...) {                                                                                                                 \
      APPDEBUGLOG("... exception");                                                                                                 \
      response->set_ret_code(-1);                                                                                                   \
      response->set_res_info("Unknown exception");                                                                                  \
    }                                                                                                                               \
  }                                                                                                                                 \

namespace order_server {

void QueryServiceImpl::query_name(::google::protobuf::RpcController* controller,
                       const ::queryNameReq* request,
                       ::queryNameRes* response,
                       ::google::protobuf::Closure* done){

  CALL_RPC_INTERFACE(QueryNameInterface);
}

void QueryServiceImpl::query_age(::google::protobuf::RpcController* controller,
                       const ::queryAgeReq* request,
                       ::queryAgeRes* response,
                       ::google::protobuf::Closure* done){

  CALL_RPC_INTERFACE(QueryAgeInterface);
}



}