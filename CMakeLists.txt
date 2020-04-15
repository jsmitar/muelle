cmake_minimum_required(VERSION 3.1)
project(Muelle LANGUAGES CXX)

# ESCAPE CHARACTER
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
if (GIT_VERSION)
  string(STRIP ${GIT_VERSION} GIT_VERSION)
else()
  set(GIT_VERSION "unknown")
endif()

string(STRIP ${GIT_COMMIT} GIT_COMMIT)

# Extra CMake Modules
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECMakeSettings)

# VARIABLES
set(CMAKE_BUILD_TYPE RelWithDebInfo)
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
set(MUELLE_DATADIR ${KDE_INSTALL_FULL_DATADIR}/muelle CACHE STRING "smith")

# PACKAGES: Qt-KDE Packages
find_package(Qt5 REQUIRED COMPONENTS Core Test Quick QuickTest X11Extras)
find_package(KF5 REQUIRED COMPONENTS WindowSystem Plasma Config)

# PACKAGES: xcb
find_package(XCB REQUIRED COMPONENTS XCB EVENT XFIXES RENDER SHAPE XINPUT)

# TARGET: configure
add_custom_target(configure
  SOURCES
  "${PACKAGE_MUELLE}/dockconfig.hpp.in"
  "${PACKAGE_MUELLE}/resources/muelle.desktop.in"
)

configure_file(
  "${PACKAGE_MUELLE}/dockconfig.hpp.in"
  "${PACKAGE_MUELLE}/dockconfig.hpp"
)

configure_file(
  "${PACKAGE_DIR}/resources/muelle.desktop.in"
  "${PACKAGE_DIR}/resources/muelle.desktop"
)

# TARGET: muelle
file(GLOB_RECURSE SOURCES RELATIVE "${CMAKE_SOURCE_DIR}/" "${PACKAGE_MUELLE}/*.cpp")
file(GLOB_RECURSE HEADERS RELATIVE "${CMAKE_SOURCE_DIR}/" "${PACKAGE_MUELLE}/*.hpp")

add_executable(muelle ${HEADERS} ${SOURCES})
target_link_libraries(muelle
  Qt5::Core
  Qt5::Quick
  Qt5::X11Extras
  KF5::WindowSystem
  KF5::ConfigCore
  KF5::Plasma
  ${XCB_LIBRARIES}
)
install(TARGETS muelle DESTINATION ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

# TARGET: qml targets
add_custom_target(shell SOURCES "${SHELL_QRC}" "${SHELL_DIST_QRC}")
add_subdirectory(${PACKAGE_DIR}/resources)

# TARGET: qml tests
add_executable(test_qml
  "${PACKAGE_DIR}/tests/qml/test_qml.cpp"
  "${PACKAGE_MUELLE}/helpers.hpp"
)
target_link_libraries(test_qml
  Qt5::Core
  Qt5::Quick
  Qt5::Test
  Qt5::QuickTest
)

# Info Messages
message(STATUS " ")
message(STATUS " -- MUELLE --")
message(STATUS "Datadir: ${MUELLE_DATADIR}")
message(STATUS "Version: ${GIT_VERSION}")
message(STATUS "Commit: ${GIT_COMMIT}")
message(STATUS " ")
