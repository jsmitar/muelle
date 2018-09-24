#ifndef DOCKVIEW_HPP
#define DOCKVIEW_HPP

#include "layout.hpp"
#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickView>

namespace Dock {
class View : public QQuickView {
  Q_OBJECT
  Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

public:
  View();
  virtual ~View() override;

  void load();

  bool containsMouse() const;
  void setContainsMouse(bool containsMouse);

signals:
  void containsMouseChanged();
  void entered();
  void exited();

protected:
  bool event(QEvent *ev) override;

private:
  bool init = false;
  Dock::Layout mLayout;
  bool mContainsMouse = false;
};

} // namespace Dock

#endif // DOCKVIEW_CPP
