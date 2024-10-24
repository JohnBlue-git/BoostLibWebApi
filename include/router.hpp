#pragma once

#include <regex>
#include <unordered_map>
#include <boost/beast.hpp>

#include "../include/controllers/person_controller.hpp"

namespace http = boost::beast::http;

class Router {
private:
  std::unordered_map<std::string, std::shared_ptr<IController>> routes;

public:
  Router(const Router&) = delete;
  Router& operator=(const Router&) = delete;
  Router(Router&& other) = delete;
  Router& operator=(Router&& other) = delete;
public:
  Router() {}
  ~Router() {}

public:
  void addRoute(const std::string &URL, std::shared_ptr<IController> controller);
  bool contains(const std::string &URL);
  std::shared_ptr<IController> getController(const std::string &URL);

private:
	bool isURL(const std::string &URL);
	std::string removeId(const std::string& URL);
};
