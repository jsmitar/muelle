#include "../../muelle/helpers.hpp"
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQuickTest>

class Setup : public QObject {
  Q_OBJECT

public:
  Setup() {}

public slots:
  void qmlEngineAvailable(QQmlEngine *engine) {
    Muelle::registerExtensions(*engine);
  }
};

QUICK_TEST_MAIN_WITH_SETUP(test_qml, Setup)

#include "test_qml.moc"