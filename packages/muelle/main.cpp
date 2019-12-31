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
#include "docktypes.hpp"
#include "dockview.hpp"
#include "helpers.hpp"
#include "layout.hpp"
#include "libs/enhancedqmlengine.hpp"
#include "libs/print_parser.hpp"
#include "libs/propertymap.hpp"
#include "libs/qmlsourcewatcher.hpp"
#include "libs/qobjectpropertyvaluesource.hpp"
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

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);
  QQmlDebuggingEnabler qmldebug;
  Dock::qmlRegisterTypes();
  qmlRegisterPressureDetector();
  qmlRegisterPanelBehavior();
  qmlRegisterPropertyMap();
  qmlRegisterQObjectPropertyValueSource();

  qInfo() << "Version:" << MUELLE_VERSION;
  qInfo() << "Commit:" << MUELLE_COMMIT;

  QQuickWindow::setDefaultAlphaBuffer(true);
  auto engine = QSharedPointer<EnhancedQmlEngine>::create();
  const auto helpers = new Dock::Helpers();
  engine->rootContext()->setContextProperty("$helpers", helpers);
  engine->setObjectOwnership(helpers, QQmlEngine::JavaScriptOwnership);

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
    std::cout << "\033c" << std::endl;
    return;
  }

  auto file = format(context);

  if (file.compare(msg.toStdString()) < 0) {
    file = "";
  }

  switch (type) {
  case QtDebugMsg:
    std::cout << "[debug] " << msg.toStdString() << " " << file << std::endl;
    break;
  case QtInfoMsg:
    std::cout << "[info] " << msg.toStdString() << " " << std::endl;
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

