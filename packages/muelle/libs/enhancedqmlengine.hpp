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

#ifndef ENHANCEDQMLENGINE_HPP
#define ENHANCEDQMLENGINE_HPP

#include "../dockconfig.hpp"
#include "rccwatcher.hpp"

#include <QObject>
#include <QPixmapCache>
#include <QPointer>
#include <QQmlEngine>
#include <QJSEngine>
#include <QQuickWindow>
#include <QResource>

class EnhancedQmlEngine : public QQmlEngine {
  Q_OBJECT
public:
  explicit EnhancedQmlEngine(QObject *parent = nullptr);

public slots:
  void loadResources();

signals:
  void sourceChanged();
  void clearSource();

private:
  Dock::RCCWatcher *mWatcher;
};

#endif // ENHANCEDQMLENGINE_HPP
