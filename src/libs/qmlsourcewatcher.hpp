#ifndef QMLPROJECTWATCHER_HPP
#define QMLPROJECTWATCHER_HPP

#include <QDir>
#include <QFileSystemWatcher>
#include <QObject>
#include <QQmlEngine>
#include <QTimer>

namespace Dock {
class QmlSourceWatcher : public QFileSystemWatcher {
  Q_OBJECT
public:
  explicit QmlSourceWatcher(const QString &qmldir, QObject *parent = nullptr);
signals:
  void sourceChanged();

private:
  void recursiveWatchPaths(QFileInfoList &files);

private:
  QString qmldir;
  QTimer debounce;
};
} // namespace Dock
#endif // QMLPROJECTWATCHER_HPP
