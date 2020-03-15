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

EnhancedQmlEngine::EnhancedQmlEngine(QObject *parent) : QQmlEngine(parent) {

#ifdef BUILD_TYPE_Debug
  mWatcher = new Muelle::RCCWatcher(SHELL_RCC, this);
  connect(mWatcher, &Muelle::RCCWatcher::rccChanged, [&] {
    emit clearSource();
    QMetaObject::invokeMethod(this, "loadResources", Qt::QueuedConnection);
  });
#else
  loadResources();
#endif
}

void EnhancedQmlEngine::loadResources() {
  trimComponentCache();
  clearComponentCache();
  collectGarbage();
  QPixmapCache::clear();
  malloc_trim(0);

  if (QResource::registerResource(MUELLE_SHELL_RCC)) {
    qInfo() << "\u001b[1;32m[watch] QML Resources Updated\u001b[0m";
    mReady = true;
  } else {
    qWarning() << "\u001b[1;32m[watch] QML Resources fail to register\u001b[0m"
               << MUELLE_SHELL_RCC;
    mReady = false;
  }
  emit sourceChanged();
}

bool EnhancedQmlEngine::ready() const { return mReady; }
