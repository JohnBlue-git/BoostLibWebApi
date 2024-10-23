#pragma once

#include "abstract_server.hpp"

class AsyncAcceptServer : public AbstractServer {
public:
  AsyncAcceptServer() = delete;
  AsyncAcceptServer(const AsyncAcceptServer&) = delete;
  AsyncAcceptServer& operator=(const AsyncAcceptServer&) = delete;
  AsyncAcceptServer(AsyncAcceptServer&& other) = delete;
  AsyncAcceptServer& operator=(AsyncAcceptServer&& other) = delete;
public:
  AsyncAcceptServer(short port, std::shared_ptr<Router> router)
    : AbstractServer(port, router) {}
  virtual ~AsyncAcceptServer() {}

public:
  void run() override;
private:
  void async_run();
};
