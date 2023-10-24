/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * ./test.cc
 * 2023-10-16 15:23:06
 * Generated by rocket framework rocket_generator.py
 * Do not edit !!!
****************************************************/


#include <google/protobuf/service.h>
#include <exception>
#include <rocket/common/log.h>
#include "./test/pb/./test.pb.h"
#include "./test/service/./test.h"
#include "./test/comm/business_exception.h"
#include "./test/interface/est.h"


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

namespace ./test {

void testserviceImpl::test(::google::protobuf::RpcController* controller,
                       const ::testreq* request,
                       ::testres* response,
                       ::google::protobuf::Closure* done){

  CALL_RPC_INTERFACE(testInterface);
}



}