#include "person_serializer.hpp"

#ifdef BOOST_JSON

boost::json::object PersonSerializer::toJson(const Person &person) {
  boost::json::object obj;
  obj["id"] = person.getId();
  obj["name"] = person.getName();
  obj["age"] = person.getAge();
  return obj;
}

Person PersonSerializer::fromJson(const boost::json::object &obj) {
  std::string name = obj.at("name").as_string().c_str();
  unsigned int age = obj.at("age").as_int64();
  return Person{name, age};
}

#else

nlohmann::json PersonSerializer::toJson(const Person &person) {
  nlohmann::json obj;
  obj["id"] = person.getId();
  obj["name"] = person.getName();
  obj["age"] = person.getAge();
  return obj;
}

Person PersonSerializer::fromJson(const nlohmann::json &obj) {
  std::string name = obj["name"];
  unsigned int age = obj["age"];
  return Person{name, age};
}

#endif