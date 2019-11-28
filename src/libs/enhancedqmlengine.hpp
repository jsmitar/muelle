#ifndef ENHANCEDQMLENGINE_HPP
#define ENHANCEDQMLENGINE_HPP

#include "../dockconfig.hpp"
#include "qmlsourcewatcher.hpp"

#include <QObject>
#include <QPointer>
#include <QProcess>
#include <QQmlEngine>
#include <QResource>

class EnhancedQmlEngine : public QQmlEngine {
  Q_OBJECT
public:
  explicit EnhancedQmlEngine(QObject *parent = nullptr);

signals:
  void sourceChanged();
  void clearSource();

private:
  void buildQmlQrc();

  Dock::QmlSourceWatcher *mWatcher;
  QProcess *mRcc;
};

#endif // ENHANCEDQMLENGINE_HPP
