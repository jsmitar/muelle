/* Muelle
 * Copyright (C) 2019 Smith Ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "dockconfig.hpp"
#include "dockcontainer.hpp"
#include "docktypes.hpp"
#include "helpers.hpp"
#include "layout.hpp"
#include "libs/enhancedqmlengine.hpp"
#include "libs/propertymap.hpp"
#include "libs/qobjectpropertyvaluesource.hpp"
#include "libs/rccwatcher.hpp"
#include "libs/rectangle.hpp"
#include "panelbehavior.hpp"
#include "pressuredetector.hpp"
#include "viewcontrast.hpp"
#include "viewshadows.hpp"

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

#include <KDBusService>

void customMessageOutput(QtMsgType type, const QMessageLogContext &context,
                         const QString &msg);

int main(int argc, char *argv[]) {
  std::ios::sync_with_stdio(true);
  qInstallMessageHandler(customMessageOutput);

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QApplication app(argc, argv);
  // app.setApplicationName("muelle");
  // app.setOrganizationDomain("org.muelle");
  QQmlDebuggingEnabler qmldebugging;
  QQuickWindow::setDefaultAlphaBuffer(true);

  qInfo() << "Version:" << MUELLE_VERSION;
  qInfo() << "Commit:" << MUELLE_COMMIT;

  Muelle::qmlRegisterTypes();
  qmlRegisterPressureDetector();
  qmlRegisterPanelBehavior();
  qmlRegisterPropertyMap();
  qmlRegisterQObjectPropertyValueSource();
  qmlRegisterViewShadows();
  qmlRegisterRectangle();
  qmlRegisterViewContrast();

  auto muelle = Muelle::Container::instance();
  KDBusService service(KDBusService::Unique);
  if (service.isRegistered()) {
    muelle->loadConfiguration();
  }

  return QApplication::exec();
}

inline std::string format(const QMessageLogContext &context) {
  auto file = std::string(context.file ? context.file : "");
  auto line = std::to_string(context.line);

  if (file.empty())
    return "";

  file = file + ":" + line + " ";

  return file.compare(0, 3, "qrc") == 0 ? file : "file://" + file;
}

void customMessageOutput(QtMsgType type, const QMessageLogContext &context,
                         const QString &msg) {

  auto file = format(context);

  if (file.compare(msg.toStdString()) < 0) {
    file = "";
  }

  switch (type) {
  case QtDebugMsg:
    std::cout << "\u001b[1;32m[debug]\u001b[0m " << msg.toStdString() << " "
              << std::endl;
    break;
  case QtInfoMsg:
    std::cout << "\u001b[1;34m[info]\u001b[0m " << msg.toStdString() << " "
              << std::endl;
    break;
  case QtWarningMsg:
    std::cout << "\u001b[1;33m[warning]\u001b[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  case QtCriticalMsg:
    std::cout << "\u001b[1;31m[critical]\u001b[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  case QtFatalMsg:
    std::cout << "\u001b[1;31m[faltal]\u001b[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  }
}
