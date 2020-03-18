#include "rectangle.hpp"
#include <QDebug>

namespace Muelle {

//! BEGIN: Rectangle

Rectangle::Rectangle(QQuickItem *parent) noexcept
    : QQuickPaintedItem(parent), mRadius(new RadiusGroup(this)) {
  // connect(mRadius, &RadiusGroup::changed, this, &Rectangle::updatePath);
  // connect(this, &Rectangle::widthChanged, this, &Rectangle::updatePath);
  // connect(this, &Rectangle::heightChanged, this, &Rectangle::updatePath);
}

Rectangle::~Rectangle() noexcept {};

RadiusGroup *Rectangle::radius() noexcept { return mRadius; }

QColor Rectangle::color() const noexcept { return mColor; };
void Rectangle::setColor(const QColor &color) noexcept {
  mColor = color;
  emit colorChanged();
}

template <typename V> constexpr auto min(const V &v) { return v; }

template <typename L, typename R, typename... Tail>
constexpr auto min(const L &l, const R &r, const Tail &... tail) {
  return min(l < r ? l : r, tail...);
}

void Rectangle::updatePath() noexcept {
  const auto w = width();
  const auto h = height();
  const auto maxRadius = min(w, h) * 2;
  const auto topLeft = min(mRadius->topLeft() * 2, maxRadius);
  const auto topRight = min(mRadius->topRight() * 2, maxRadius);
  const auto bottomLeft = min(mRadius->bottomLeft() * 2, maxRadius);
  const auto bottomRight = min(mRadius->bottomRight() * 2, maxRadius);

  mPath.clear();

  // topLeft
  if (topLeft) {
    mPath.arcTo(0, 0, topLeft, topLeft, 90, 90);
  } else {
    mPath.lineTo(0, 0);
  }

  // bottomLeft
  if (bottomLeft) {
    mPath.arcTo(0, h - bottomLeft, bottomLeft, bottomLeft, 180, 90);
  } else {
    mPath.lineTo(0, h);
  }

  // bottomRight
  if (bottomRight) {
    mPath.arcTo(w - bottomRight, h - bottomRight, bottomRight, bottomRight, 270,
                90);
  } else {
    mPath.lineTo(w, h);
  }

  // bottomLeft
  if (topRight) {
    mPath.arcTo(w - topRight, 0, topRight, topRight, 0, 90);
  } else {
    mPath.lineTo(w, 0);
  }

  mPath.lineTo(topRight / 2, 0);
  mPath.closeSubpath();
}

void Rectangle::paint(QPainter *paint) {
  updatePath();
  paint->setRenderHint(QPainter::Antialiasing);
  paint->setPen(Qt::PenStyle::NoPen);
  paint->setBrush(mColor);
  paint->drawPath(mPath);
}

//! END: Rectangle

} // namespace Muelle