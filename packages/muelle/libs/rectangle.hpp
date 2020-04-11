#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include <QColor>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QQmlEngine>
#include <QQuickPaintedItem>

namespace Muelle {

class RadiusGroup : public QObject {
  Q_OBJECT
  Q_PROPERTY(int topLeft READ topLeft WRITE setTopLeft NOTIFY changed);
  Q_PROPERTY(int topRight READ topRight WRITE setTopRight NOTIFY changed);
  Q_PROPERTY(int bottomLeft READ bottomLeft WRITE setBottomLeft NOTIFY changed);
  Q_PROPERTY(
      int bottomRight READ bottomRight WRITE setBottomRight NOTIFY changed);

public:
  RadiusGroup(QObject *parent = nullptr) noexcept : QObject(parent) {}
  ~RadiusGroup() noexcept {};

  inline void setTopLeft(int radius) noexcept {
    mTopLeft = radius;
    emit changed();
  }

  inline int topLeft() const noexcept { return mTopLeft; }

  inline void setTopRight(int radius) noexcept {
    mTopRight = radius;
    emit changed();
  }

  inline int topRight() const noexcept { return mTopRight; }

  inline void setBottomLeft(int radius) {
    mBottomLeft = radius;
    emit changed();
  }

  inline int bottomLeft() const noexcept { return mBottomLeft; }

  inline void setBottomRight(int radius) noexcept {
    mBottomRight = radius;
    emit changed();
  }

  inline int bottomRight() const noexcept { return mBottomRight; }

signals:
  void changed();

private:
  int mTopLeft{0};
  int mTopRight{0};
  int mBottomLeft{0};
  int mBottomRight{0};
};

class Rectangle : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(Muelle::RadiusGroup *radius MEMBER mRadius)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
  Rectangle(QQuickItem *parent = 0) noexcept;
  ~Rectangle() noexcept;

  RadiusGroup *radius() noexcept;

  QColor color() const noexcept;
  void setColor(const QColor &color) noexcept;

  void updatePolish() override;

  void paint(QPainter *painter) override;

signals:
  void colorChanged();

private:
  RadiusGroup *mRadius;
  QColor mColor;
  QPainterPath mPath;
};

} // namespace Muelle

inline void qmlRegisterRectangle() {
  qmlRegisterType<Muelle::Rectangle>("org.muelle.extra", 1, 0, "Rectangle");
  qmlRegisterUncreatableType<Muelle::RadiusGroup>(
      "org.muelle.extra", 1, 0, "RadiusGroup",
      "RadiusGroup is an uncreatable type");
}
#endif