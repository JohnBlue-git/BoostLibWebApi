#pragma once

#include "interface_controller.hpp"
#include "../serializers/person_serializer.hpp"
#include "../services/person_service.hpp"

class PersonController : public IController {
private:
  std::shared_ptr<IPersonService> personService;

public:
  PersonController() = delete;
  PersonController(const PersonController&) = delete;
  PersonController& operator=(const PersonController&) = delete;
  PersonController(PersonController&& other) = delete;
  PersonController& operator=(PersonController&& other) = delete;
public:
  PersonController(std::shared_ptr<IPersonService> service)
      : personService(service) {}
  virtual ~PersonController() {}

public:
  void handleRequest(Context& ctx) override;

private:
  std::tuple<http::status, std::string> getPersons();
  std::tuple<http::status, std::string> createPerson(const http::request<http::string_body>& req);
  std::tuple<http::status, std::string> getPersonById(int id);
  std::tuple<http::status, std::string> deletePersonById(int id);
};
