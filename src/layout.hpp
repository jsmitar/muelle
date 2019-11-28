#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "docktypes.hpp"
#include <QDebug>
#include <QFlags>
#include <QObject>
#include <QQmlEngine>
#include <QtGlobal>

///*
//   Assign and emit a signal
//   M: Member, V: new value, S: signal
//*/
//#define DOCK_ASSIGN(M, V) \
//  if (M != V) { \
//    M = V; \
//  }

namespace Dock {
class Layout : public QObject {
  Q_OBJECT
  Q_DISABLE_COPY(Layout)
  Q_PROPERTY(Dock::Types::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)
  Q_PROPERTY(Dock::Types::Orientation orientation READ orientation NOTIFY
                 orientationChanged)
  Q_PROPERTY(Dock::Types::Alignment alignment READ alignment WRITE setAlignment
                 NOTIFY alignmentChanged)
  Q_PROPERTY(uint layout READ layout NOTIFY layoutChanged)
  Q_PROPERTY(bool isHorizontal READ isHorizontal NOTIFY orientationChanged)
  Q_PROPERTY(bool isVertical READ isVertical NOTIFY orientationChanged)

public:
  explicit Layout(QObject *parent = nullptr);
  virtual ~Layout();

  constexpr Types::Edge edge() const;
  constexpr Types::Orientation orientation() const;
  constexpr Types::Alignment alignment() const;
  constexpr uint layout() const;

  constexpr bool isHorizontal() const;
  constexpr bool isVertical() const;

  void setEdge(Types::Edge edge);
  void setAlignment(Types::Alignment alignment);

signals:
  void edgeChanged();
  void orientationChanged();
  void alignmentChanged();
  void layoutChanged();

private:
  Types::Orientation mOrientation = Types::Orientation::Horizontal;
  Types::Edge mEdge = Types::Edge::Bottom;
  Types::Alignment mAlignment = Types::Alignment::Center;
};

constexpr Types::Edge Layout::edge() const { return mEdge; }

constexpr Types::Orientation Layout::orientation() const {
  return mOrientation;
}

constexpr Types::Alignment Layout::alignment() const { return mAlignment; }

constexpr uint Layout::layout() const {
  return static_cast<uint>(mEdge) | static_cast<uint>(mOrientation) |
         static_cast<uint>(mAlignment);
}

constexpr bool Layout::isHorizontal() const {
  return mOrientation == Types::Orientation::Horizontal;
}

constexpr bool Layout::isVertical() const {
  return mOrientation == Types::Orientation::Vertical;
}
} // namespace Dock
#endif // LAYOUT_HPP
