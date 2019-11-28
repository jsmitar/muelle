#include "enhancedqmlengine.hpp"
#include <QDebug>
#include <QtGlobal>
#include <malloc.h>

EnhancedQmlEngine::EnhancedQmlEngine(QObject *parent) : QQmlEngine(parent) {
  mRcc = new QProcess(this);

  connect(mRcc, qOverload<int, QProcess::ExitStatus>(&QProcess::finished),
          [&](int exitCode) {
            if (exitCode == 0) {
              if (QResource::registerResource(QML_RCC)) {
                qInfo() << "[HQR] Qml Resources loaded";
                emit sourceChanged();
              }
            }
          });

#ifdef BUILD_TYPE_Debug
  // enable hot-reloading on debug build
  mWatcher = new Dock::QmlSourceWatcher(QStringLiteral(QML_SOURCE_DIR), this);
  connect(mWatcher, &Dock::QmlSourceWatcher::sourceChanged, [&] {
    if (mRcc->state() != QProcess::NotRunning)
      mRcc->kill();

    qInfo() << "[CLEAR]";
    emit clearSource();
    clearComponentCache();
    collectGarbage();
    malloc_trim(0);
    if (!QResource::unregisterResource(QML_RCC))
      qInfo() << "[HQR] Qml Resources no unloaded";

    buildQmlQrc();
  });

  buildQmlQrc();
#else
  if (!QResource::registerResource(QML_RCC)) {
    buildQmlQrc();
  }
#endif
}

void EnhancedQmlEngine::buildQmlQrc() {
  mRcc->waitForFinished();
  mRcc->start(RCC, {"--binary", QML_QRC, "-o", QML_RCC});
}
