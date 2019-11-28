#ifndef DOCKVIEW_HPP
#define DOCKVIEW_HPP

#include "behavior.hpp"
#include "layout.hpp"
#include "libs/enhancedqmlengine.hpp"
#include "positionhandler.hpp"

#include <memory>

#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QPointer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>
#include <QRect>
#include <QSharedPointer>

typedef void onEventFunc(QEvent *);

namespace Dock {
class View : public QQuickView {
  Q_OBJECT
  Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
  Q_PROPERTY(QRect mask READ mask WRITE setMask NOTIFY maskChanged)
  Q_PROPERTY(QRect geometry READ geometry NOTIFY maskChanged)
  Q_PROPERTY(QRect panelGeometry READ panelGeometry WRITE setPanelGeometry
                 NOTIFY panelGeometryChanged)
  Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(QPoint mousePosition READ mousePosition)

  public:
  View(QSharedPointer<EnhancedQmlEngine> &engine);
  ~View() override;
  void load();

  bool containsMouse() const;
  void setContainsMouse(bool containsMouse);
  void setMask(const QRect &region);
  const Layout &layout() const;
  QRect mask() const;
  QRect geometry() const;
  QRect panelGeometry() const;
  QRect absolutePanelGeometry() const;
  void setPanelGeometry(const QRect &value);
  void setPosition(const QPoint &value);
  void setSize(const QSize &size);
  QPoint mousePosition() const;

  Q_INVOKABLE void setOpacity(qreal level);

signals:
  void containsMouseChanged();
  void entered();
  void exited();
  void maskChanged();
  void positionChanged();
  void sizeChanged();
  void panelGeometryChanged();

protected:
  bool event(QEvent *ev) override;

private:
  bool init = false;
  bool mContainsMouse = false;
  Dock::Layout mLayout;
  Dock::Behavior mBehavior;
  Dock::PositionHandler mPositionHandler;
  QSharedPointer<EnhancedQmlEngine> mEngine;
  QRect mPanelGeometry;
};

} // namespace Dock

#endif // DOCKVIEW_CPP
