// clang-format off
#ifndef DOCKCONFIG_HPP
#define DOCKCONFIG_HPP

#define MUELLE_VERSION "v0.1pre"
#define MUELLE_COMMIT "c02e024"
#define BUILD_TYPE_Debug
#define SOURCE_DIR "/home/_audoban/Dev/desktop-projects/Muelle"
#define QML_INSTALL_DIR ""
#define QML_SOURCE_DIR "/home/_audoban/Dev/desktop-projects/Muelle/qml"
#define QML_QRC "/home/_audoban/Dev/desktop-projects/Muelle/qml/qml.qrc"
#define QML_RCC "/home/_audoban/Dev/desktop-projects/Muelle/qml.rcc"
#define RCC ""

#ifdef BUILD_TYPE_Debug
#  ifndef QT_SHAREDPOINTER_TRACK_POINTERS
#    define QT_SHAREDPOINTER_TRACK_POINTERS
#  endif
#endif

#endif // DOCKCONFIG_HPP
