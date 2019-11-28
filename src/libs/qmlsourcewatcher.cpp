#include "qmlsourcewatcher.hpp"
#include "../dockconfig.hpp"
#include "functions.hpp"
#include <QDebug>

namespace Dock {

constexpr auto filters{QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot |
                       QDir::Readable | QDir::Writable};

QmlSourceWatcher::QmlSourceWatcher(const QString &qmldir, QObject *parent)
    : QFileSystemWatcher(parent), qmldir(qmldir) {

  connect(this, &QmlSourceWatcher::directoryChanged, [&](auto subdir) {
    auto files = QDir(subdir).entryInfoList(filters);
    recursiveWatchPaths(files);
    debounce.start(300);
  });

  debounce.setSingleShot(true);
  connect(this, &QmlSourceWatcher::fileChanged, [&] { debounce.start(300); });
  connect(&debounce, &QTimer::timeout, this, &QmlSourceWatcher::sourceChanged);

  addPath(qmldir);
  auto files = QDir(qmldir).entryInfoList(filters);
  recursiveWatchPaths(files);
}

void QmlSourceWatcher::recursiveWatchPaths(QFileInfoList &files) {
  for (auto &f : files) {
    auto match = [&f](auto &... str) {
      return (f.filePath().endsWith(str) || ...);
    };

    if (f.isSymLink())
      continue;

    if (f.isFile() && f.isReadable() &&
        match(".qml", ".js", ".qrc", ".qmldir", ".conf")) {
      addPath(f.filePath());

    } else if (f.isDir() && f.isAbsolute()) {
      auto files = QDir(f.filePath()).entryInfoList(filters);
      addPath(f.filePath());
      recursiveWatchPaths(files);
    }
  }
}
} // namespace Dock
