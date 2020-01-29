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
#include "panelbehavior.hpp"
#include "pressuredetector.hpp"

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

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
  QApplication app(argc, argv);
  QQmlDebuggingEnabler qmldebugging;
  Muelle::qmlRegisterTypes();
  qmlRegisterPressureDetector();
  qmlRegisterPanelBehavior();
  qmlRegisterPropertyMap();
  qmlRegisterQObjectPropertyValueSource();

  QQuickWindow::setDefaultAlphaBuffer(true);

  qInfo() << "Version:" << MUELLE_VERSION;
  qInfo() << "Commit:" << MUELLE_COMMIT;

  auto muelle = Muelle::Container();
  muelle.loadConfiguration();

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
    std::cout << "\033[1;32m[debug]\033[0m " << msg.toStdString() << " "
              << std::endl;
    break;
  case QtInfoMsg:
    std::cout << "\033[1;34m[info]\033[0m " << msg.toStdString() << " "
              << std::endl;
    break;
  case QtWarningMsg:
    std::cout << "\033[1;33m[warning]\033[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  case QtCriticalMsg:
    std::cout << "\033[1;31m[critical]\033[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  case QtFatalMsg:
    std::cout << "\033[1;31m[faltal]\033[0m " << msg.toStdString() << " "
              << file << std::endl;
    break;
  }
}
