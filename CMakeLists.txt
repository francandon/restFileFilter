cmake_minimum_required(VERSION 3.14)

project(restFileSystemIndexer)

message("===============  ${PROJECT_NAME}  ==============")

set(CMAKE_CXX_STANDARD 17)

# Find ROOT
find_package(ROOT REQUIRED)
execute_process(COMMAND root-config --cflags OUTPUT_VARIABLE ROOT_CFLAGS)
string(STRIP ${ROOT_CFLAGS} ROOT_CFLAGS)
message(
    "-- Found ROOT version: ${ROOT_VERSION} with compilation flags: ${ROOT_CFLAGS} and libraries: ${ROOT_LIBRARIES}"
)

# Find PostgreSQL library (apt install -y libpq-dev postgresql-server-dev-all)
find_package(PostgreSQL REQUIRED)

# Find REST
find_package(REST REQUIRED)
message(
    "-- Found REST version: ${REST_VERSION} with libraries: ${REST_LIBRARIES}")

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)

add_executable(${PROJECT_NAME} main.cpp ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} ${ROOT_LIBRARIES} ${PostgreSQL_LIBRARIES}
                      ${REST_LIBRARIES} ROOT::RHTTP)

target_include_directories(
    ${PROJECT_NAME} SYSTEM
    PUBLIC ${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS}
           ${PostgreSQL_INCLUDE_DIRS} ${REST_INCLUDE_DIRS})

target_compile_options(${PROJECT_NAME} PRIVATE)

file(GLOB scripts "database/scripts/*")
file(COPY ${scripts} DESTINATION database/scripts)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)