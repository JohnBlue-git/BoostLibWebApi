#pragma once

#include <regex>
#include <string>
#include <tuple>
#include <cstdlib>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#if defined(BOOST_JSON)
  #include <boost/json.hpp>
#endif

#include "../context.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http;   // from <boost/beast/http.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>
using tcp = net::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

#define GET (http::verb::get)
#define POST (http::verb::post)
#define PUT (http::verb::put)
#define PATCH (http::verb::patch)
#define DELETE (http::verb::delete_)

class IController {
public:
  virtual void handleRequest(Context& ctx) = 0;
};
