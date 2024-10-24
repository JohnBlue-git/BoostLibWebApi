#include "router.hpp"

bool Router::isURL(const std::string &URL) {
  std::smatch matches;
  std::regex pattern(R"(^/[^/]+(?:/[^/]+)?(?:/(\d+))?$)");
  return std::regex_match(URL, matches, pattern);
}

std::string Router::removeId(const std::string& URL) {
    std::regex pattern(R"(^(/[^/]+(?:/[^/]+)?)(?:/\d+)?$)");
    return std::regex_replace(URL, pattern, "$1");
}

void Router::addRoute(const std::string &URL, std::shared_ptr<IController> controller) {
  const std::string subURL = removeId(URL);
  if ( URL.compare(subURL) != 0 || false == isURL(subURL) ) {
		perror("registed URL format incorrect.");
		return;
  }
  else if ( true == Router::contains(subURL) ) {
		perror("registed URL already exist.");
		return;
  }
  routes[subURL] = controller;
}

bool Router::contains(const std::string &URL) {
  if ( false == isURL(URL) ) {
		return false;
  }
	return routes.contains(removeId(URL));
}

std::shared_ptr<IController> Router::getController(const std::string &URL) {
  const std::string subURL = removeId(URL);
  if ( false == isURL(subURL) ) {
		return nullptr;
  }
	if ( false == routes.contains(subURL) ) {
		return nullptr;
	}
	return routes[subURL];
}
