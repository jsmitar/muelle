// clang-format off
#ifndef DOCKCONFIG_HPP
#define DOCKCONFIG_HPP

#define MUELLE_VERSION "0.0.1"
#define MUELLE_COMMIT "2a6c7d6"
#define SHELL_RCC "/home/smith/dev/muelle/packages/dist/shell/qml.rcc"
#define SOURCE_DIR "/home/smith/dev/muelle"

#define BUILD_TYPE_Debug
#ifdef BUILD_TYPE_Debug
#  ifndef QT_SHAREDPOINTER_TRACK_POINTERS
#    define QT_SHAREDPOINTER_TRACK_POINTERS
#  endif
#endif

#endif // DOCKCONFIG_HPP
