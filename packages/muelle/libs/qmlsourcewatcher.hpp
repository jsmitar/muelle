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

#ifndef QMLPROJECTWATCHER_HPP
#define QMLPROJECTWATCHER_HPP

#include <QDir>
#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>

namespace Dock {
class QmlSourceWatcher : public QFileSystemWatcher {
  Q_OBJECT

public:
  explicit QmlSourceWatcher(const QString &qmldir, QObject *parent = nullptr);

signals:
  void sourceChanged();

private:
  void recursiveWatchPaths(QFileInfoList &files);

private:
  QString qmldir;
  QTimer debounce;
};
} // namespace Dock
#endif // QMLPROJECTWATCHER_HPP
