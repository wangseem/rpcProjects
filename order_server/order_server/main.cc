/****************************************************
 *
 * ****     ***     ****    *   *    *****    *****
 * *  *    *   *   *        ****     ***        *
 * *   *    ***     ****    *   *    *****      *
 *
 * order_server.h
 * 2023-08-29 16:25:22
 * Generated by rocket framework rocket_generator.py
 * Do not edit !!!
****************************************************/




#include <google/protobuf/service.h>
#include <memory>
#include <rocket/common/log.h>
#include <rocket/common/config.h>
#include <rocket/net/tcp/net_addr.h>
#include <rocket/net/tcp/tcp_server.h>
#include <rocket/net/rpc/rpc_dispatcher.h>
#include "service/order_server.h"

int main(int argc, char* argv[]) {

  if (argc != 2) {
    printf("Start order_server error, argc not 2 \n");
    printf("Start like this: \n");
    printf("./order_server ../conf/rocket.xml \n");
    return 0;
  }

  rocket::Config::SetGlobalConfig(argv[1]);

  rocket::Logger::InitGlobalLogger();

  rocket::RpcDispatcher::GetRpcDispatcher()->registerService(std::make_shared<order_server::QueryServiceImpl>());

  rocket::IPNetAddr::s_ptr addr = std::make_shared<rocket::IPNetAddr>("127.0.0.1", rocket::Config::GetGlobalConfig()->m_port);

  rocket::TcpServer tcp_server(addr);

  tcp_server.start();

  return 0;
}