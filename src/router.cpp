#include "../include/router.hpp"

bool Router::isURL(const std::string &URL) {
  std::smatch matches;
  std::regex pattern(R"(^/[^/]+(?:/[^/]+)?(?:/(\d+))?$)");
  return std::regex_match(URL, matches, pattern);
}

std::string Router::removeNumberFromURL(const std::string& URL) {
    std::regex pattern(R"(^(/[^/]+(?:/[^/]+)?)(?:/\d+)?$)");
    return std::regex_replace(URL, pattern, "$1");
}

void Router::addRoute(const std::string &URL, std::shared_ptr<IController> controller) {
  if ( false == isURL(URL) ) {
		perror("URL format incorrect.");
		return;
  }
  else if ( true == Router::contains(URL) ) {
		perror("URL already exist.");
		return;
  }
  routes[URL] = controller;
}

bool Router::contains(const std::string &URL) {
  if ( false == isURL(URL) ) {
		return false;
  }
	return routes.contains(removeNumberFromURL(URL));
}

std::shared_ptr<IController> Router::getController(const std::string &URL) {
  if ( false == isURL(URL) ) {
		return nullptr;
  }
	if ( false == routes.contains(removeNumberFromURL(URL)) ) {
		return nullptr;
	}
	return routes[URL];
}
