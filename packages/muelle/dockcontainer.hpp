#ifndef DOCKCONTAINER_H
#define DOCKCONTAINER_H

#include "dockview.hpp"
#include "helpers.hpp"
#include "libs/enhancedqmlengine.hpp"

#include "dockview.hpp"
#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <Plasma/Theme>
#include <QApplication>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QQmlEngine>
#include <QScreen>
#include <QUuid>
#include <QVector>
#include <algorithm>

using UUID = QString;

namespace Muelle {
class Container : public QObject {
  Q_OBJECT
public:
  explicit Container(QObject *parent = nullptr);

  void loadConfiguration();

  bool loadView(const UUID &uuid);
  void unloadView(const UUID &uuid);

  void addView(QScreen *screen);
  void removeView(const UUID &view);

  KConfigGroup configuration(const UUID &view);

  UUID createDefaultView();

  QScreen *findScreen(const QString &name) const;

  Plasma::Theme *theme();

  static Container *instance() noexcept;

private:
  QMap<UUID, View *> mViews;
  EnhancedQmlEngine *mEngine;
  Plasma::Theme *mTheme;
  KSharedConfig::Ptr mConfig;
};
} // namespace Muelle
#endif // DOCKCONTAINER_H
