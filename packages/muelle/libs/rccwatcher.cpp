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

#include "rccwatcher.hpp"
#include "../dockconfig.hpp"
#include <QDebug>

namespace Dock {

constexpr auto filters{QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot |
                       QDir::Readable | QDir::Writable};

RCCWatcher::RCCWatcher(const QString &rcc, QObject *parent)
    : QFileSystemWatcher(parent) {

  debounce.setSingleShot(true);
  debounce.setInterval(300);

  connect(&debounce, &QTimer::timeout, this, &RCCWatcher::rccChanged);
  connect(this, &RCCWatcher::fileChanged, [this, rcc] {
    qDebug() << "[watch] RCC file changed";
    addPath(rcc);
    debounce.start();
  });
  addPath(rcc);
  debounce.start();
}

} // namespace Dock
