#include "layout.hpp"

namespace Dock {

Layout::Layout(QObject *parent) : QObject(parent) {
  setObjectName(QStringLiteral("Dock::Layout"));
  connect(this, &Layout::edgeChanged, this, &Layout::layoutChanged);
  connect(this, &Layout::alignmentChanged, this, &Layout::layoutChanged);
}

Layout::~Layout() {}

void Layout::setEdge(Types::Edge edge) {
  switch (edge) {
  case Types::Edge::Top:
  case Types::Edge::Bottom:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != Types::Orientation::Horizontal) {
        mOrientation = Types::Orientation::Horizontal;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  case Types::Edge::Left:
  case Types::Edge::Right:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != Types::Orientation::Vertical) {
        mOrientation = Types::Orientation::Vertical;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  }
}

void Layout::setAlignment(Types::Alignment alignment) {
  if (mAlignment != alignment) {
    mAlignment = alignment;
    emit alignmentChanged();
  }
}
} // namespace Dock
