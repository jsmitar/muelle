#include "src/dockconfig.hpp"
#include "src/docktypes.hpp"
#include "src/dockview.hpp"
#include "src/layout.hpp"
#include "src/libs/enhancedqmlengine.hpp"
#include "src/libs/print_parser.hpp"
#include "src/libs/propertymap.hpp"
#include "src/libs/qmlsourcewatcher.hpp"
#include "src/pressuredetector.hpp"

#include <cstring>
#include <iostream>
#include <string>

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QQmlDebuggingEnabler>
#include <QQmlEngine>
#include <QQuickWindow>
#include <QSharedPointer>
#include <QTimer>

void customMessageOutput(QtMsgType type, const QMessageLogContext &context,
                         const QString &msg);

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(true);
  qInstallMessageHandler(customMessageOutput);

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  QQmlDebuggingEnabler qmldebug;
  Dock::qmlRegisterTypes();
  qmlRegisterPressureDetector();
  qmlRegisterPropertyMap();

  qDebug() << "Version:" << DUCKDOCK_VERSION;
  qDebug() << "Commit:" << DUCKDOCK_COMMIT;

  QQuickWindow::setDefaultAlphaBuffer(true);
  auto engine = QSharedPointer<EnhancedQmlEngine>::create();

  Dock::View view(engine);

  return QApplication::exec();
}

inline std::string format(const QMessageLogContext &context) {
  auto file = std::string(context.file ? context.file : "");
  auto line = std::to_string(context.line);

  if (file.empty())
    return "";

  file = std::strlen(SOURCE_DIR) > file.length()
           ? file + ":" + line + " "
           : file.substr(std::strlen(SOURCE_DIR)) + ":" + line + " ";

  return file.compare(0, 3, "qrc") == 0 ? file : "file:" + file;
}

void customMessageOutput(QtMsgType type, const QMessageLogContext &context,
                         const QString &msg) {

  if (msg.startsWith("[CLEAR]")) {
    // std::cout << "\033c" << std::endl;
    std::cout << "__________________________________" << std::endl;
    return;
  }

  auto file = msg.contains("[HQR]") ? "" : format(context);

  if (file.compare(msg.toStdString()) < 0) {
    file = "";
  }

  switch (type) {
  case QtDebugMsg:
    std::cout << "[debug] " << msg.toStdString() << " " << file << std::endl;
    break;
  case QtInfoMsg:
    std::cout << "[info] " << msg.toStdString() << " " << file << std::endl;
    fflush(stdout);
    break;
  case QtWarningMsg:
    std::cout << "[warning] " << msg.toStdString() << " " << file << std::endl;
    break;
  case QtCriticalMsg:
    std::cout << "[critical] " << msg.toStdString() << " " << file << std::endl;
    break;
  case QtFatalMsg:
    std::cout << "[faltal] " << msg.toStdString() << " " << file << std::endl;
    break;
  }
}
