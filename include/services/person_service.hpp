#pragma once

#include <vector>

#include "../models/person.hpp"

class IPersonService {
public:
  virtual void addPerson(const Person &person) = 0;
  virtual std::vector<Person> getAllPersons() = 0;
  virtual std::optional<Person> getPersonById(unsigned int id) = 0;
  virtual bool deletePersonById(unsigned int id) = 0;
};

class PersonService : public IPersonService {
private:
  std::vector<Person> persons;

public:
  void addPerson(const Person &person) override;
  std::vector<Person> getAllPersons() override;
  std::optional<Person> getPersonById(unsigned int id) override;
  bool deletePersonById(unsigned int id) override;
};