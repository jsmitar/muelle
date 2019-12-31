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
#ifdef BUILD_TYPE_Debug
  // enable hot-reloading on debug build
  mWatcher = new Dock::RCCWatcher(SHELL_RCC, this);
  connect(mWatcher, &Dock::RCCWatcher::sourceChanged, [&] {
    qInfo() << "[CLEAR]";
    emit clearSource();
    QResource::unregisterResource(SHELL_RCC);

    trimComponentCache();
    clearComponentCache();
    collectGarbage();
    QPixmapCache::clear();
    malloc_trim(0);

    qInfo() << "[watch] Building sources";
    if (QResource::registerResource(SHELL_RCC)) {
      qInfo() << "[watch] Qml Resources loaded";
      emit sourceChanged();
    }
  });

  if (QResource::registerResource(SHELL_RCC)) {
    qInfo() << "[watch] Qml Resources loaded";
    emit sourceChanged();
  }
#else
  QResource::registerResource(SHELL_RCC);
#endif
}
