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
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "" FORCE)

set(PACKAGE_DIR "${CMAKE_SOURCE_DIR}/packages")
set(PACKAGE_MUELLE "${CMAKE_SOURCE_DIR}/packages/muelle")
set(PACKAGE_SHELL "${CMAKE_SOURCE_DIR}/packages/dist/shell")
set(PACKAGE_SHARED "${CMAKE_SOURCE_DIR}/packages/dist/shared")
set(SHELL_QRC "packages/shell.qrc")
set(SHELL_DIST_QRC "packages/dist/shell.qrc")
set(SHELL_RCC "packages/resources/shell.rcc")

# Packages: Qt-KDE Packages
find_package(Qt5 REQUIRED COMPONENTS Core X11Extras Quick)
find_package(KF5WindowSystem REQUIRED)

# Packages: xcb
find_package(XCB REQUIRED COMPONENTS XCB EVENT XFIXES RENDER SHAPE XINPUT)

# Target: configure
add_custom_target(configure SOURCES "${PACKAGE_MUELLE}/dockconfig.hpp.in")
configure_file(
  "${PACKAGE_MUELLE}/dockconfig.hpp.in" 
  "${PACKAGE_MUELLE}/dockconfig.hpp"
)

# Target: muelle
file(GLOB_RECURSE SOURCES RELATIVE "${CMAKE_SOURCE_DIR}/" "${PACKAGE_MUELLE}/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_SOURCE_DIR}/" "${PACKAGE_MUELLE}/*.hpp")

add_executable(muelle ${HEADERS} ${SOURCES})
target_link_libraries(muelle
  Qt5::Core
  Qt5::Quick
  Qt5::X11Extras
  KF5::WindowSystem
  ${XCB_LIBRARIES}
)

# Target: qml targets
add_custom_target(shell SOURCES "${SHELL_QRC}" "${SHELL_DIST_QRC}")

# Info Messages
message(STATUS " ")
message(STATUS "Muelle project")
message(STATUS "Version: ${GIT_VERSION}")
message(STATUS "Commit: ${GIT_COMMIT}")
message(STATUS " ")
