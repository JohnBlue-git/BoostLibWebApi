#include "context.hpp"

http::request<http::string_body>& Context::getRequest() { return request; }

http::response<http::string_body>& Context::getResponse() { return response; }

void Context::setJsonResponse(http::status status, const std::string &body) {
  response.result(status);
  response.set(http::field::content_type, "application/json");
  response.body() = body;
  response.prepare_payload(); // Ensure headers like Content-Length are set
}

//void Context::setJsonRequest( ? , const std::string &body) {}
