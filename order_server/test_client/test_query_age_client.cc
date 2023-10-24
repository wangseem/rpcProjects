/*
 * @Description: 
 * @Author: kevin
 * @Date: 2023-08-29 16:25:22
 * @LastEditors: Please set LastEditors
 * @LastEditTime: 2023-10-16 16:08:02
 */
/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * test_query_age_client.cc
 * 2023-08-29 16:25:22
 * Generated by rocket framework rocket_generator.py
****************************************************/

#include <google/protobuf/service.h>
#include <rocket/net/rpc/rpc_channel.h>
#include <rocket/net/rpc/rpc_controller.h>
#include <rocket/net/rpc/rpc_closure.h>
#include <rocket/common/log.h>
#include <stdio.h>
#include "order_server/pb/order_server.pb.h"


void test_client(const std::string& addr) {

  NEWMESSAGE(::queryAgeReq, request);
  NEWMESSAGE(::queryAgeRes, response);

  // request->set_xxx(100);

  NEWRPCCONTROLLER(controller);
  controller->SetTimeout(2000);

  std::shared_ptr<rocket::RpcClosure> closure = std::make_shared<rocket::RpcClosure>(nullptr, [request, response, controller]() mutable {
    if (controller->GetErrorCode() == 0) {
      INFOLOG("call rpc success, request[%s], response[%s]", request->ShortDebugString().c_str(), response->ShortDebugString().c_str());
      
    } else {
      ERRORLOG("call rpc failed, request[%s], error code[%d], error info[%s]", 
        request->ShortDebugString().c_str(), 
        controller->GetErrorCode(), 
        controller->GetErrorInfo().c_str());
    }

    exit(0);
  });

  CALLRPRC(addr, QueryService_Stub, query_age, controller, request, response, closure);

} 

int main(int argc, char* argv[]) {

  if (argc != 3) {
    printf ("param error, Usage: ./client ip port\n");
    printf ("For example: ./client 127.0.0.1 12345\n");
    return -1;
  }

  rocket::Config::SetGlobalConfig(NULL);

  rocket::Logger::InitGlobalLogger(0);

  std::string addr = std::string(argv[1]) + ":" + std::string(argv[2]); 
  std::cout << "*******************************************" <<std::string(argv[1]) <<":" << std::string(argv[2]);

  test_client(addr);

  return 0;
}
