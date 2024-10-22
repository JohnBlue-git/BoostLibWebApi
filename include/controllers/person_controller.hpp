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
#include "../serializers/person_serializer.hpp"
#include "../services/person_service.hpp"

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

class PersonController : public IController {
private:
  std::shared_ptr<IPersonService> personService;

public:
  PersonController(std::shared_ptr<IPersonService> service)
      : personService(service) {}

public:
  void handleRequest(Context& ctx) override;

private:
  std::tuple<http::status, std::string> getPersons();
  std::tuple<http::status, std::string> createPerson(const http::request<http::string_body>& req);
  std::tuple<http::status, std::string> getPersonById(int id);
  std::tuple<http::status, std::string> deletePersonById(int id);
};
