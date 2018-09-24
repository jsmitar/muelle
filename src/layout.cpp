#include "layout.hpp"

Dock::Layout::Layout(QObject *parent) : QObject(parent) {

  setObjectName(QStringLiteral("Dock.Layout"));
  connect(this, &Layout::edgeChanged, this, &Layout::layoutChanged);
  connect(this, &Layout::alignmentChanged, this, &Layout::layoutChanged);
}

Dock::DockType::Edge Dock::Layout::edge() const { return mEdge; }
Dock::DockType::Orientation Dock::Layout::orientation() const {
  return mOrientation;
}

Dock::DockType::Alignment Dock::Layout::alignment() const { return mAlignment; }

uint Dock::Layout::layout() const {
  return static_cast<uint>(mEdge) | static_cast<uint>(mOrientation) |
         static_cast<uint>(mAlignment);
}

void Dock::Layout::setEdge(Dock::DockType::Edge edge) {
  switch (edge) {
  case DockType::Edge::Top:
  case DockType::Edge::Bottom:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != DockType::Orientation::Horizontal) {
        mOrientation = DockType::Orientation::Horizontal;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  case DockType::Edge::Left:
  case DockType::Edge::Right:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != DockType::Orientation::Vertical) {
        mOrientation = DockType::Orientation::Vertical;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  }
}

void Dock::Layout::setAlignment(Dock::DockType::Alignment alignment) {
  if (mAlignment != alignment) {
    mAlignment = alignment;
    emit alignmentChanged();
  }
}
