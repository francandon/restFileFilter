cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(PostgresExample VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the PostgreSQL client library
find_package(PostgreSQL REQUIRED)

# Specify the include directories
include_directories(inc ${PostgreSQL_INCLUDE_DIRS})

# Add the shared library
add_library(postgres SHARED src/PostgresDB.cpp)

# Link the PostgreSQL client library
target_link_libraries(postgres ${PostgreSQL_LIBRARIES})

# Specify the executable for testing
add_executable(main main.cpp)

# Link the executable to the library
target_link_libraries(main postgres)
