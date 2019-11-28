#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "docktypes.hpp"

#include <QObject>
#include <QRect>
#include <QtGui/qwindowdefs.h>

namespace Dock {
class View;
class BehaviorPrivate;

class Behavior : public QObject {
  Q_OBJECT
  Q_PROPERTY(Dock::Types::Behavior behavior READ behavior WRITE setBehavior
                 NOTIFY behaviorChanged)
  Q_PROPERTY(bool dodge READ dodge NOTIFY dodgeChanged)

public:
  explicit Behavior(View *parent = nullptr);
  ~Behavior();

  Types::Behavior behavior() const;
  void setBehavior(Types::Behavior behavior);

  bool dodge() const;

  void setParentWindow(WId wid) const;

signals:
  void behaviorChanged();
  void dodgeChanged();
  void showingDesktop(bool showing);

private:
  BehaviorPrivate *dPtr;
};

} // namespace Dock

#endif // BEHAVIOR_H
