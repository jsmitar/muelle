#include "behavior.hpp"
#include "behavior_p.hpp"
#include "dockview.hpp"

namespace Dock {
Behavior::Behavior(View *parent) : dPtr(new BehaviorPrivate(this)) {

  connect(parent, &Dock::View::panelGeometryChanged,
          [this, parent]() { dPtr->setGeometry(parent->panelGeometry()); });
}

Behavior::~Behavior() { delete dPtr; }

Types::Behavior Behavior::behavior() const { return dPtr->behavior; }

void Behavior::setBehavior(Types::Behavior behavior) {
  dPtr->setBehavior(behavior);
  emit behaviorChanged();
}

bool Behavior::dodge() const { return dPtr->nextDodgeValue; }

void Behavior::setParentWindow(WId wid) const { dPtr->parentWId = wid; }
} // namespace Dock
