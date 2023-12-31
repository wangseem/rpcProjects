/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * interface.h
 * 2023-10-16 15:23:06
 * Generated by rocket framework rocket_generator.py
 * File will not generate while exist
 * Allow editing
****************************************************/

#ifndef ./TEST_INTERFACE_H
#define ./TEST_INTERFACE_H 

#include <rocket/net/rpc/rpc_closure.h>
#include <rocket/net/rpc/rpc_controller.h>
#include <rocket/net/rpc/rpc_interface.h>
#include <google/protobuf/message.h>

namespace ./test {

/*
 * Rpc Interface Base Class
 * All interface should extend this abstract class
*/

class Interface : public rocket::RpcInterface {
 public:

  Interface(const google::protobuf::Message* req, google::protobuf::Message* rsp, rocket::RpcClosure* done, rocket::RpcController* controller);

  virtual ~Interface() = 0;

};


}


#endif