/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * interface.cc
 * 2023-08-29 16:25:22
 * Generated by rocket framework rocket_generator.py
 * File will not generate while exist
 * Allow editing
****************************************************/


#include <rocket/common/log.h>
#include <rocket/net/rpc/rpc_closure.h>
#include <rocket/net/rpc/rpc_closure.h>
#include <rocket/net/rpc/rpc_controller.h>
#include <google/protobuf/message.h>

#include "order_server/interface/interface.h"

namespace order_server {


Interface::Interface(const google::protobuf::Message* req, google::protobuf::Message* rsp, rocket::RpcClosure* done, rocket::RpcController* controller)
  : rocket::RpcInterface(req, rsp, done, controller) {

}

Interface::~Interface() {

}

}