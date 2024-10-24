#include "person_controller.hpp"

void PersonController::handleRequest(Context& ctx) {
  const http::request<http::string_body>& req = ctx.getRequest();

  const std::string path(req.target());
  std::smatch matches;
  std::regex pattern(R"(^/[^/]+(?:/[^/]+)?(?:/(\d+))?$)");
  std::regex_match(path, matches, pattern);

  http::status status;
  std::string jsonStr;

	if (matches[1].matched) { // Check if ID exists
		int id = std::stoi(matches[1]); // Extracting the {id}

		if (req.method() == GET) {
      std::tie(status, jsonStr) = PersonController::getPersonById(id);
		}
		else if (req.method() == DELETE) {
			std::tie(status, jsonStr) = PersonController::deletePersonById(id);
		}
		//else if (req.method() == PUT) {
		//	PersonController::putPersonById(req, id);
		//}
    else {
      status = http::status::not_found;
      jsonStr = "{\"error\": \"Method not found.\"}";
    }
	}
	else if (req.method() == GET) {
    std::tie(status, jsonStr) = PersonController::getPersons();
	}
	else if (req.method() == POST) {
		std::tie(status, jsonStr) = PersonController::createPerson(req);
	}
  else {
    status = http::status::not_found;
    jsonStr = "{\"error\": \"Method not found.\"}";
  }

  ctx.setJsonResponse(status, jsonStr);
}

std::tuple<http::status, std::string> PersonController::getPersons() {
  std::string jsonString;

  try {
    auto persons = personService->getAllPersons();
#ifdef BOOST_JSON
    boost::json::array jsonArray;
    for (const auto &person : persons) {
      jsonArray.push_back(PersonSerializer::toJson(person));
    }
    jsonString = boost::json::serialize(jsonArray);
#else
    nlohmann::json jsonArray;
    for (const auto &person : persons) {
      jsonArray.push_back(PersonSerializer::toJson(person));
    }
    jsonString = jsonArray.dump();
#endif

    return { http::status::ok, jsonString };
  }
  catch (const std::exception &e) {
    return { http::status::internal_server_error, "{\"error\": \"Failed to serialize persons.\"}" };
  }
}

std::tuple<http::status, std::string> PersonController::createPerson(const http::request<http::string_body>& req) {

  try {
#ifdef BOOST_JSON
    auto json = boost::json::parse(req.body());
    auto person = PersonSerializer::fromJson(json.as_object());
#else
    auto json = nlohmann::json::parse(req.body());
    auto person = PersonSerializer::fromJson(json);
#endif

    personService->addPerson(person);
    
    return { http::status::created, "{\"success\": \"Person created.\"}" };
  }
  catch (const std::exception &e) {
    return { http::status::bad_request, "{\"error\": \"Invalid JSON payload.\"}" };
  }
}

std::tuple<http::status, std::string> PersonController::getPersonById(int id) {
  std::string jsonString;

  try {
    auto person = personService->getPersonById(id);

    if (person) {
#ifdef BOOST_JSON
      jsonString = boost::json::serialize(PersonSerializer::toJson(*person)); // return optional 用法
      //jsonString = boost::json::serialize(PersonSerializer::toJson(person.value())); // return optional 用法
#else
      jsonString = PersonSerializer::toJson(*person).dump(); // return optional 用法
      //jsonString = PersonSerializer::toJson(person.value()).dump(); // return optional 用法
#endif

      return { http::status::ok, jsonString };
    }
    else {
      return { http::status::not_found, "{\"error\": \"Person not found.\"}" };
    }
  }
  catch (const std::exception &e) {
    return { http::status::internal_server_error, "{\"error\": \"Failed to serialize persons.\"}" };
  }
}

std::tuple<http::status, std::string> PersonController::deletePersonById(int id) {

  try {
    if (personService->deletePersonById(id)) {
      return { http::status::no_content, "" };
    }
    else {
      return { http::status::not_found, "{\"error\": \"Person not found.\"}" };
    }
  }
  catch (const std::exception &e) {
    return { http::status::internal_server_error, "{\"error\": \"Failed to delete person.\"}" };
  }
}
