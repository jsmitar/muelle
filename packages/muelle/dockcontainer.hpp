#ifndef DOCKCONTAINER_H
#define DOCKCONTAINER_H

#include "dockview.hpp"
#include "helpers.hpp"
#include "libs/debouncesignal.hpp"
#include "libs/enhancedqmlengine.hpp"

#include <KConfig>
#include <KConfigGroup>
#include <KSharedConfig>
#include <Plasma/Theme>
#include <QAbstractNativeEventFilter>
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

#include <QtX11Extras/QX11Info>
#include <xcb/randr.h>
#include <xcb/xcb.h>
#include <xcb/xcb_event.h>

using UUID = QString;

namespace Muelle {
class Container : public QObject, public QAbstractNativeEventFilter {
  Q_OBJECT
  Q_PROPERTY(QVariantMap screens READ screensVariant NOTIFY screensChanged)

public:
  explicit Container(QObject *parent = nullptr);

  void updateScreens();

  void loadConfiguration();

  void reloadViews();
  bool loadView(const UUID &uuid);
  void unloadView(const UUID &uuid);

  KConfigGroup configuration(const UUID &view);

  UUID createDefaultView();

  QScreen *findScreen(const QString &name) const;

  Plasma::Theme *theme();

  static Container *instance() noexcept;

  QVariantMap screensVariant() const noexcept;

  bool nativeEventFilter(const QByteArray &eventType, void *message,
                         long *) override;

signals:
  void screensChanged();
  void nativeScreensChanged();

private:
  QMap<UUID, View *> mViews;
  EnhancedQmlEngine *mEngine;
  Plasma::Theme *mTheme;
  KSharedConfig::Ptr mConfig;
  QVariantMap mScreens;
};
} // namespace Muelle
#endif // DOCKCONTAINER_H
