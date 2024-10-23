#!/bin/bash

curl -X POST -H "Content-Type: application/json" -d '{"name": "John Doe", "age": 30}' http://localhost:1999/api/person -v

curl -X GET -H "Content-Type: application/json" http://localhost:1999/api/person -v

curl -X PUT -H "Content-Type: application/json" -d '{"name": "John Doe", "age": 30}' http://localhost:1999/api/person -v

curl http://localhost:1999/api/person -v

curl http://localhost:1999/42 -v

curl http://localhost:1999/api/person/0 -v

curl http://localhost:1999/api/person/42 -v
