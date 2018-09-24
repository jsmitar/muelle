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
    Top = 1 << 1,
    Right = 1 << 2,
    Bottom = 1 << 3,
    Left = 1 << 4,
  };
  Q_ENUM(Edge)

  enum class Orientation {
    Horizontal = 1 << 5,
    Vertical = 1 << 6,
  };
  Q_ENUM(Orientation)

  enum class Alignment {
    Start = 1 << 7,
    Center = 1 << 8,
    End = 1 << 9,
  };
  Q_ENUM(Alignment)
};

inline void qmlRegisterDockType() {
  qmlRegisterUncreatableType<DockType>("org.duckdock.types", 1, 0, "DockType",
                                       "Dock is a uncreatable type");
}
} // namespace Dock
#endif // DOCKTYPE_HPP
