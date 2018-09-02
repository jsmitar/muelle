cmake_minimum_required(VERSION 3.1)
project(DuckDock LANGUAGES CXX)

execute_process(
  COMMAND git describe --abbrev=0 --tags
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_VERSION
)
string(STRIP ${GIT_VERSION} GIT_VERSION)

execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT_HASH
)
string(STRIP ${GIT_COMMIT_HASH} GIT_COMMIT_HASH)

set(DuckDock_VERSION ${GIT_VERSION})
set(DuckDock_COMMIT_HASH ${GIT_COMMIT_HASH})

message(STATUS "Version: ${GIT_VERSION}")
message(STATUS "Commit hash: ${GIT_COMMIT_HASH}")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QT_QML_DEBUG ON)

configure_file(
  "${PROJECT_SOURCE_DIR}/src/dockconfig.hpp.in"
  "${PROJECT_SOURCE_DIR}/src/dockconfig.hpp"
)

find_package(Qt5 COMPONENTS Core Quick REQUIRED)

file(GLOB_RECURSE SOURCES "${PROJECT_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE HEADERS "${PROJECT_SOURCE_DIR}" src/*.hpp)
file(GLOB CONFIGURE src/*.*.in)

add_executable(${PROJECT_NAME} "${HEADERS}" "${SOURCES}" "main.cpp" "shell/qml.qrc")
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Quick)

add_custom_target(CONFIGURE SOURCES ${CONFIGURE})
