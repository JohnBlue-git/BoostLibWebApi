#pragma once

#include <map>
#include <boost/beast.hpp>
#include <boost/beast/http.hpp>

namespace http = boost::beast::http;

class Context {
private:
  http::request<http::string_body> request;
  http::response<http::string_body> response;

public:
  Context(const Context&) = delete;
  Context& operator=(const Context&) = delete;
  Context(Context&& other) = delete;
  Context& operator=(Context&& other) = delete;
public:
  Context() {}
  ~Context() {}

  // Getters for the request and response to provide read/write access
  http::request<http::string_body>& getRequest();
  http::response<http::string_body>& getResponse();

  // Add more functionality as needed, such as setting response status, headers,
  // etc. For example, a helper function to easily set the response result and
  // body
  void setJsonResponse(http::status status, const std::string &body);

  //void setJsonRequest( ? , const std::string &body);
};
