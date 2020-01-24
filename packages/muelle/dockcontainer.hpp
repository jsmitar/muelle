#ifndef DOCKCONTAINER_H
#define DOCKCONTAINER_H

#include "libs/enhancedqmlengine.hpp"

#include <QObject>
#include <QQmlEngine>
#include <QVector>

namespace Dock {
class DockContainer : public QObject {
  Q_OBJECT
public:
  explicit DockContainer(QObject *parent = nullptr);

signals:

private:
  EnhancedQmlEngine *engine;
};
} // namespace Dock
#endif // DOCKCONTAINER_H
