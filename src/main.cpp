#include "dockconfig.hpp"
#include "docktype.hpp"
#include "dockview.hpp"
#include "layout.hpp"

#include <QDebug>
#include <QGuiApplication>
#include <QQmlDebuggingEnabler>

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

  QGuiApplication app(argc, argv);
  QQmlDebuggingEnabler enabler;
  Dock::qmlRegisterDockType();

  Dock::View view;
  view.load();

  qDebug() << "Version:" << DuckDock_VERSION;
  qDebug() << "Commit Hash:" << DuckDock_COMMIT_HASH;

  return QGuiApplication::exec();
}
