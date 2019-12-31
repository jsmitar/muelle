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

#include "qmlsourcewatcher.hpp"
#include "../dockconfig.hpp"
#include "functions.hpp"
#include <QDebug>

namespace Dock {

constexpr auto filters{QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot |
                       QDir::Readable | QDir::Writable};

QmlSourceWatcher::QmlSourceWatcher(const QString &qmldir, QObject *parent)
    : QFileSystemWatcher(parent), qmldir(qmldir) {

  connect(this, &QmlSourceWatcher::directoryChanged, [&](auto subdir) {
    auto files = QDir(subdir).entryInfoList(filters);
    recursiveWatchPaths(files);
    debounce.start(300);
  });

  debounce.setSingleShot(true);
  connect(this, &QmlSourceWatcher::fileChanged, [&] { debounce.start(300); });
  connect(&debounce, &QTimer::timeout, this, &QmlSourceWatcher::sourceChanged);

  addPath(qmldir);
  auto files = QDir(qmldir).entryInfoList(filters);
  recursiveWatchPaths(files);
}

void QmlSourceWatcher::recursiveWatchPaths(QFileInfoList &files) {
  for (auto &f : files) {
    auto match = [&f](auto &... str) {
      return (f.filePath().endsWith(str) || ...);
    };

    if (f.isSymLink())
      continue;

    if (f.isFile() && f.isReadable() &&
        match(".qml", ".js", ".qrc", ".qmldir", ".conf")) {
      addPath(f.filePath());

    } else if (f.isDir() && f.isAbsolute()) {
      auto files = QDir(f.filePath()).entryInfoList(filters);
      addPath(f.filePath());
      recursiveWatchPaths(files);
    }
  }
}
} // namespace Dock
