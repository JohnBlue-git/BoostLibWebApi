cmake_minimum_required(VERSION 3.10)

# Project
project(WebApi VERSION 1.0)

# C++ flag
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Pthread flag
set(THREADS_PREFER_PTHREAD_FLAG ON)



# Include header
include_directories(WebApi PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include/controllers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/models
  ${CMAKE_CURRENT_SOURCE_DIR}/include/serializers
  ${CMAKE_CURRENT_SOURCE_DIR}/include/server
  ${CMAKE_CURRENT_SOURCE_DIR}/include/services
  )
# Source code
file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp
  )
# Executable
add_executable(WebApi ${SOURCES})
# Additional flags
target_compile_options(WebApi PRIVATE
  -Wall -Wpedantic -Wextra
  )



# Find Thread (could be pthread or win-thread)
find_package(Threads REQUIRED)
target_link_libraries(WebApi Threads::Threads)



# Find Boost
find_package(Boost 1.71.0 REQUIRED COMPONENTS system)
if(Boost_VERSION VERSION_GREATER_EQUAL "1.75.0")
  # Boost Jason start from 1.75.0
  find_package(Boost 1.75.0 REQUIRED COMPONENTS system json)
  # Define BOOST_JSON
  add_definitions(-DBOOST_JSON)
endif()
target_include_directories(WebApi PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(WebApi ${Boost_LIBRARIES})

# Add compile definitions globally
#add_compile_definitions( ... )

# Add boost library compile definitions
#
# DBOOST_ASIO_DISABLE_THREADS: Disables threading in Boost.Asio, making it single-threaded.
# DBOOST_ALL_NO_LIB: Prevents automatic linking of Boost libraries, requiring manual linking instead.
# DBOOST_SYSTEM_NO_DEPRECATED: Suppresses deprecated features in the Boost.System library.
# DBOOST_ERROR_CODE_HEADER_ONLY: Makes Boost error codes header-only.
# DBOOST_COROUTINES_NO_DEPRECATION_WARNING: Suppresses deprecation warnings for Boost Coroutines.
target_compile_definitions(WebApi PRIVATE
    BOOST_ASIO_DISABLE_THREADS
    BOOST_ALL_NO_LIB
    BOOST_SYSTEM_NO_DEPRECATED
    BOOST_ERROR_CODE_HEADER_ONLY
    BOOST_COROUTINES_NO_DEPRECATION_WARNING
)

# Add defines (for selecting type of server)
option(USE_ASYNC_ACCEPT "Use async accept server" OFF)
option(USE_BLOCK_ACCEPT "Use block accept server" ON)
if (USE_ASYNC_ACCEPT)
  target_compile_definitions(WebApi PRIVATE ASYNC_ACCEPT)
elseif (USE_BLOCK_ACCEPT)
  target_compile_definitions(WebApi PRIVATE BLOCK_ACCEPT)
else()
  target_compile_definitions(WebApi PRIVATE BLOCK_ACCEPT)
endif()
