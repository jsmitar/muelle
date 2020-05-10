#ifndef DOCKCONTAINER_H
#define DOCKCONTAINER_H

#include "dockview.hpp"
#include "helpers.hpp"
#include "libs/enhancedqmlengine.hpp"

#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <Plasma/Theme>
#include <QApplication>
#include <QGuiApplication>
#include <QMap>
#include <QMetaObject>
#include <QObject>
#include <QQmlEngine>
#include <QScreen>
#include <QUuid>
#include <QVariant>
#include <QVector>
#include <algorithm>

using UUID = QString;

namespace Muelle {
class Container : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariantMap screens READ screensVariant NOTIFY screensChanged)

public:
  explicit Container(QObject *parent = nullptr);

  void loadConfiguration();

  bool loadView(const UUID &uuid);
  void unloadView(const UUID &uuid);

  void loadViews(QScreen *screen);
  void unloadViews(QScreen *screen);

  void addView(QScreen *screen);
  void removeView(const UUID &view);

  KConfigGroup configuration(const UUID &view);

  UUID createDefaultView();

  QScreen *findScreen(const QString &name) const;

  Plasma::Theme *theme();

  static Container *instance() noexcept;

  QVariantMap screensVariant() const noexcept;

signals:
  void screensChanged();

private:
  QMap<UUID, View *> mViews;
  EnhancedQmlEngine *mEngine;
  Plasma::Theme *mTheme;
  KSharedConfig::Ptr mConfig;
  QVariantMap mScreens;
};
} // namespace Muelle
#endif // DOCKCONTAINER_H
