#pragma once

#include "interface_server.hpp"

class AbstractServer : public IServer {
protected:
  unsigned short port;
  std::shared_ptr<Router> router;
  net::io_context io_context{1};
  tcp::acceptor acpt;

public:
  AbstractServer() = delete;
  AbstractServer(const AbstractServer&) = delete;
  AbstractServer& operator=(const AbstractServer&) = delete;
  AbstractServer(AbstractServer&& other) = delete;
  AbstractServer& operator=(AbstractServer&& other) = delete;
public:
  AbstractServer(short port, std::shared_ptr<Router> router);
  virtual ~AbstractServer();

public:
  void session(tcp::socket socket) override;
  void run() override;
  short getPort() override;
};
