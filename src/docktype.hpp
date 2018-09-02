#ifndef DOCKTYPE_HPP
#define DOCKTYPE_HPP

#include <QObject>
#include <QQmlEngine>

namespace Dock { // namespace Dock
class DockType {
  Q_GADGET
  Q_DISABLE_COPY(DockType)

public:
  enum class Edge {
    Top,
    Right,
    Bottom,
    Left,
  };
  Q_ENUM(Edge)

  enum class Orientation {
    Horizontal,
    Vertical,
  };
  Q_ENUM(Orientation)

  enum class Alignment {
    Start,
    Center,
    End,
  };
  Q_ENUM(Alignment)
};

inline void qmlRegisterDockType() {
  qmlRegisterUncreatableType<DockType>("org.duckdock", 1, 0, "Dock",
                                       "Dock is a uncreatable type");
}
} // namespace Dock
#endif // DOCKTYPE_HPP
