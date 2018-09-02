#include "./src/dockconfig.hpp"
#include "./src/docktype.hpp"
#include <QDebug>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlDebuggingEnabler>

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);
  QQmlDebuggingEnabler enabler;
  QQmlApplicationEngine engine;
  Dock::qmlRegisterDockType();

  engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

  if (engine.rootObjects().isEmpty()) {
    return -1;
  }

  qDebug() << "Version:" << DuckDock_VERSION;
  qDebug() << "Commit Hash:" << DuckDock_COMMIT_HASH;

  return QGuiApplication::exec();
}
