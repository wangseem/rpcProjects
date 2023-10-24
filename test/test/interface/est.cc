/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * est.cc
 * 2023-10-16 15:23:06
 * Generated by rocket framework rocket_generator.py
 * File will not generate while exist
 * Allow editing
****************************************************/


#include <rocket/common/log.h>
#include "./test/interface/est.h"
#include "./test/interface/interface.h"
#include "./test/pb/./test.pb.h"

namespace ./test {

testInterface::testInterface(const ::testreq* request, ::testres* response, 
    rocket::RpcClosure* done, rocket::RpcController* controller)
  : Interface(dynamic_cast<const google::protobuf::Message*>(request), dynamic_cast<google::protobuf::Message*>(response), done, controller),
    m_request(request), 
    m_response(response) {
  APPINFOLOG("In|request:{%s}", request->ShortDebugString().c_str());
}

testInterface::~testInterface() {
  APPINFOLOG("Out|response:{%s}", m_response->ShortDebugString().c_str());
}

void testInterface::run() {

  //
  // Run your business logic at here
  // 

  m_response->set_ret_code(0);
  m_response->set_res_info("OK");

}

void testInterface::setError(int code, const std::string& err_info) {
  m_response->set_ret_code(code);
  m_response->set_res_info(err_info);
}

}