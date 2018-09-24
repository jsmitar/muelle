#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "docktype.hpp"
#include <QFlags>
#include <QObject>
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
  Q_PROPERTY(
      Dock::DockType::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)
  Q_PROPERTY(Dock::DockType::Orientation orientation READ orientation NOTIFY
                 orientationChanged)
  Q_PROPERTY(Dock::DockType::Alignment alignment READ alignment WRITE
                 setAlignment NOTIFY alignmentChanged)
  Q_PROPERTY(uint layout READ layout NOTIFY layoutChanged)

public:
  explicit Layout(QObject *parent = nullptr);

  DockType::Edge edge() const;
  DockType::Orientation orientation() const;
  DockType::Alignment alignment() const;
  uint layout() const;

  void setEdge(DockType::Edge edge);
  void setAlignment(DockType::Alignment alignment);

signals:
  void edgeChanged();
  void orientationChanged();
  void alignmentChanged();
  void layoutChanged();

private:
  DockType::Edge mEdge = DockType::Edge::Bottom;
  DockType::Orientation mOrientation = DockType::Orientation::Horizontal;
  DockType::Alignment mAlignment = DockType::Alignment::Center;
};
} // namespace Dock

#endif // LAYOUT_HPP
