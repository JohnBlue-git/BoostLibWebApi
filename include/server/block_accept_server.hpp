#pragma once

#include "abstract_server.hpp"

class BlockAcceptServer : public AbstractServer {
public:
  BlockAcceptServer() = delete;
  BlockAcceptServer(const BlockAcceptServer&) = delete;
  BlockAcceptServer& operator=(const BlockAcceptServer&) = delete;
  BlockAcceptServer(BlockAcceptServer&& other) = delete;
  BlockAcceptServer& operator=(BlockAcceptServer&& other) = delete;
public:
  BlockAcceptServer(short port, std::shared_ptr<Router> router)
    : AbstractServer(port, router) {}
  virtual ~BlockAcceptServer() {}
};
