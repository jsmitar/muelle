#ifndef VIEWSHADOWS_HPP
#define VIEWSHADOWS_HPP

#include "docktypes.hpp"
#include <KWindowShadow>
#include <KWindowShadowTile>
#include <KWindowSystem>
#include <Plasma/Svg>
#include <QApplication>
#include <QMap>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QRect>
#include <QWindow>

namespace Muelle {
class ViewShadows : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QWindow *view READ view WRITE setView NOTIFY viewChanged)
  Q_PROPERTY(uint edges READ edges WRITE setEdges NOTIFY edgesChanged)
  Q_PROPERTY(
      QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
  ViewShadows(QObject *parent = nullptr) noexcept;
  ~ViewShadows() override;

  void classBegin() override;
  void componentComplete() override;

  void setView(QWindow *view);
  QWindow *view() const;

  void setGeometry(const QRect &rect);
  QRect geometry() const;

  void setEdges(uint edges);
  uint edges() const;

  void setEnabled(bool enabled);
  bool enabled() const;

private:
  void configureTiles();

signals:
  void geometryChanged();
  void enabledChanged();
  void viewChanged();
  void edgesChanged();

private:
  bool mEnabled{true};
  Types::Edges mEdges;
  QRect mRect;
  QWindow *mView{nullptr};
  KWindowShadow *mShadow;

  static Plasma::Svg *mSvg;
  static QMap<QString, KWindowShadowTile::Ptr> mTile;
  static QMap<QString, int> mHint;
};
} // namespace Muelle

inline void qmlRegisterViewShadows() {
  qmlRegisterType<Muelle::ViewShadows>("org.muelle.extra", 1, 0, "ViewShadows");
}
#endif // VIEWSHADOWS_HPP
