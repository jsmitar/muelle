cmake_minimum_required(VERSION 3.1)
project(Muelle LANGUAGES CXX)

# escape character
string(ASCII 27 Esc)

execute_process(
  COMMAND git describe --abbrev=0 --tags
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_VERSION
)
execute_process(
  COMMAND git rev-parse --short HEAD
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  OUTPUT_VARIABLE GIT_COMMIT
)
string(STRIP ${GIT_VERSION} GIT_VERSION)
string(STRIP ${GIT_COMMIT} GIT_COMMIT)

# Extra CMake Modules
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)

# Variables
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(QT_QML_DEBUG ON)
set(RCC "/usr/bin/rcc-qt5")
set(QML_IMPORT_PATH ${CMAKE_SOURCE_DIR}/qml CACHE STRING "" FORCE)
set(QML2_IMPORT_PATH ${CMAKE_SOURCE_DIR}/qml CACHE STRING "" FORCE)

# Packages: Qt-KDE Packages
find_package(Qt5 REQUIRED COMPONENTS Core X11Extras Quick)
find_package(KF5WindowSystem REQUIRED)

# Packages: xcb
find_package(XCB REQUIRED COMPONENTS XCB EVENT XFIXES XINPUT)

# Target: configure
file(GLOB CONFIGURE src/*.*.in)
add_custom_target(configure SOURCES ${CONFIGURE})
configure_file(
  "${CMAKE_SOURCE_DIR}/src/dockconfig.hpp.in"
  "${CMAKE_SOURCE_DIR}/src/dockconfig.hpp"
)

# Includes: range-v3
#include_directories("${CMAKE_SOURCE_DIR}/liab/range-v3/include")

# Target: muelle
file(GLOB_RECURSE SOURCES RELATIVE "${CMAKE_SOURCE_DIR}" src/*.cpp)
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_SOURCE_DIR}" src/*.hpp)
#file(GLOB_RECURSE RANGEV3 RELATIVE "${CMAKE_SOURCE_DIR}" lib/range-v3/include/*.hpp)

add_executable(muelle ${HEADERS} ${SOURCES} "main.cpp")
target_link_libraries(muelle
  Qt5::Core
  Qt5::Quick
  Qt5::X11Extras
  KF5::WindowSystem
  ${XCB_LIBRARIES}
)

# Target: qml
add_custom_target(qml SOURCES "qml/qml.qrc")

# Info Messages
message(STATUS " ")
message(STATUS "- Muelle project")
message(STATUS "- ${Esc}[34mVersion: ${GIT_VERSION}${Esc}[0m")
message(STATUS "- Commit: ${GIT_COMMIT}")
message(STATUS " ")
