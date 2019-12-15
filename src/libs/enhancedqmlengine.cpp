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

#include "enhancedqmlengine.hpp"
#include <QDebug>
#include <QtGlobal>
#include <malloc.h>

EnhancedQmlEngine::EnhancedQmlEngine(QObject *parent) : QQmlEngine(parent) {
  mRcc = new QProcess(this);

  connect(mRcc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
          [&](int exitCode) {
            if (exitCode == 0) {
              if (QResource::registerResource(QML_RCC)) {
                qInfo() << "[HQR] Qml Resources loaded";
                emit sourceChanged();
              }
            }
          });

#ifdef BUILD_TYPE_Debug
  // enable hot-reloading on debug build
  mWatcher = new Dock::QmlSourceWatcher(QStringLiteral(QML_SOURCE_DIR), this);
  connect(mWatcher, &Dock::QmlSourceWatcher::sourceChanged, [&] {
    if (mRcc->state() != QProcess::NotRunning)
      mRcc->kill();

    qInfo() << "[CLEAR]";
    emit clearSource();
    if (!QResource::unregisterResource(QML_RCC))
      qInfo() << "[HQR] Qml Resources no unloaded";

    trimComponentCache();
    clearComponentCache();
    collectGarbage();
    QPixmapCache::clear();
    malloc_trim(0);

    buildQmlQrc();
  });

  buildQmlQrc();
#else
  if (!QResource::registerResource(QML_RCC)) {
    buildQmlQrc();
  }
#endif
}

void EnhancedQmlEngine::buildQmlQrc() {
  mRcc->waitForFinished();
  mRcc->start(RCC, {"--binary", QML_QRC, "-o", QML_RCC});
}
