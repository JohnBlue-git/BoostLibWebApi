#pragma once

#include <iostream>
#include <map>
#include <string>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#include "context.hpp"
#include "router.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

class IServer {
public:
  virtual void session(tcp::socket socket) = 0;
  virtual void run() = 0;
  virtual short getPort() = 0;
};
