/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * query_age.h
 * 2023-08-29 16:25:22
 * Generated by rocket framework rocket_generator.py
 * File will not generate while exist
 * Allow editing
****************************************************/

#ifndef ORDER_SERVER_INTERFACE_QUERY_AGE_H
#define ORDER_SERVER_INTERFACE_QUERY_AGE_H 

#include <rocket/net/rpc/rpc_closure.h>
#include "order_server/pb/order_server.pb.h"
#include "order_server/interface/interface.h"


namespace order_server {

/*
 * Rpc Interface Class
 * Alloc one object every time RPC call begin, and destroy this object while RPC call end
*/

class QueryAgeInterface : public Interface {
 public:

  QueryAgeInterface(const ::queryAgeReq* request, ::queryAgeRes* response, 
    rocket::RpcClosure* done, rocket::RpcController* controller);

  ~QueryAgeInterface();

 public:
  // core business deal method
  virtual void run() override;

  // set error code and error into to response message
  virtual void setError(int code, const std::string& err_info) override;

 private:
  const ::queryAgeReq* m_request {NULL};       // request object fron client

  ::queryAgeRes* m_response {NULL};           // response object that reply to client

};


}


#endif