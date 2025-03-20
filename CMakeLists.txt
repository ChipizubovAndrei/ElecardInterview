cmake_minimum_required(VERSION 3.10)

project(ElecardTestTask LANGUAGES CXX)

set(CMAKE_CXX_STANDART 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB SOURCE "*.cpp" "*.h")

message(SOURCE ${SOURCES})

add_executable(${PROJECT_NAME} ${SOURCE})

target_link_libraries(${PROJECT_NAME} PRIVATE pthread)