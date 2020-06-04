#include "rectangle.hpp"
#include <QDebug>

namespace Muelle {

Gradient::Gradient(QObject *parent) noexcept : QObject(parent) {}

QLinearGradient *Gradient::toQGradient(qreal width, qreal height) noexcept {
  if (!m_gradient)
    return nullptr;

  auto f = [](qreal x, int deg) { return x * std::sin(deg * M_PI / 180); };
  auto g = [](qreal x, int deg) { return x * std::cos(deg * M_PI / 180); };

  const qreal w = width / 2;
  const qreal h = height / 2;
  const qreal deg = m_degrees;

  m_gradient->setStart(f(w, deg) + w, g(h, deg) + h);
  m_gradient->setFinalStop(-f(w, deg) + w, -g(h, deg) + h);

  return m_gradient.get();
}

void Gradient::setDegrees(int deegrees) {
  auto value = clamp(0, deegrees, 360);
  if (value != m_degrees) {
    m_degrees = value;
    emit changed();
  }
}

int Gradient::degrees() const noexcept { return m_degrees; }

void Gradient::setStops(QVariant stops) noexcept {
  if (auto value = stops.value<QJSValue>(); m_stops.strictlyEquals(value)) {
    return;
  } else {
    m_gradient.reset();
    m_stops = value;
  }

  if (!m_stops.isArray())
    return;

  m_gradient = std::make_unique<QLinearGradient>(0, 0, 0, 100);

  auto get_stop = [](QJSValue value, qreal def_position) -> QGradientStop {
    const auto stop = value.toString().split(' ');
    const auto color = QColor(stop[0]);
    auto ok = false;
    const auto position =
        stop.count() >= 2 ? clamp(0, stop[1].toDouble(&ok), 1) : def_position;

    return {ok ? position : def_position, color};
  };

  const auto length = m_stops.property(QStringLiteral("length")).toInt();

  for (size_t i = 0; i < length; i++) {
    const auto [positon, color] =
        get_stop(m_stops.property(i), static_cast<qreal>(i) / length);

    m_gradient->setColorAt(positon, color);
  }

  emit changed();
}

QVariant Gradient::stops() const noexcept {
  return QVariant::fromValue(m_stops);
}

//! BEGIN: Rectangle

Rectangle::Rectangle(QQuickItem *parent) noexcept
    : QQuickPaintedItem(parent), m_radius(new Radius(this)),
      m_border(new Border(this)), m_gradient(new Gradient(this)) {

  auto update = [this] { this->update(); };

  connect(m_radius, &Radius::changed, this, &Rectangle::updatePolish);
  connect(m_border, &Border::widthChanged, this, &Rectangle::updatePolish);
  connect(m_border, &Border::colorChanged, update);
  connect(m_border->gradient(), &Gradient::changed, update);
  connect(m_gradient, &Gradient::changed, update);
  connect(this, &Rectangle::widthChanged, this, &Rectangle::updatePolish);
  connect(this, &Rectangle::heightChanged, this, &Rectangle::updatePolish);
}

Rectangle::~Rectangle() noexcept {};

Radius *Rectangle::radius() noexcept { return m_radius; }

void Rectangle::setColor(const QColor &color) noexcept {
  m_color = color;
  emit colorChanged();
  update();
}

QColor Rectangle::color() const noexcept { return m_color; };

QPainterPath Rectangle::createPath(qreal border) {
  const qreal w = width();
  const qreal h = height();
  const qreal t = border / 2;
  const qreal l = border / 2;
  const qreal r = w - border / 2;
  const qreal b = h - border / 2;

  const qreal maxr = min(r, b) * 2 - border * 2;
  qreal tl = max(0, m_radius->topLeft() * maxr - border);
  qreal tr = max(0, m_radius->topRight() * maxr - border);
  qreal bl = max(0, m_radius->bottomLeft() * maxr - border);
  qreal br = max(0, m_radius->bottomRight() * maxr - border);

  if (w < h) {
    if (auto v = tl + tr; v > maxr) {
      auto glut = (v - maxr) / 2 + 1;
      tl = clamp(0, tl - glut, maxr);
      tr = clamp(0, tr - glut, maxr);
    }

    if (auto v = bl + br; v > maxr) {
      auto glut = (v - maxr) / 2 + 1;
      bl = clamp(0, bl - glut, maxr);
      br = clamp(0, br - glut, maxr);
    }
  } else {
    if (auto v = tl + bl; v > maxr) {
      auto glut = (v - maxr) / 2 + 1;
      tl = clamp(0, tl - glut, maxr);
      bl = clamp(0, bl - glut, maxr);
    }

    if (auto v = tr + br; v > maxr) {
      auto glut = (v - maxr) / 2 + 1;
      tr = clamp(0, tr - glut, maxr);
      br = clamp(0, br - glut, maxr);
    }
  }

  QPainterPath path;

  path.moveTo(max(tl / 2, l), t);
  // topLeft radius
  if (tl) {
    path.arcTo(l, t, tl, tl, 90, 90);
  } else {
    path.lineTo(l, t);
  }

  // bottomLeft radius
  if (bl) {
    path.arcTo(l, b - bl, bl, bl, 180, 90);
  } else {
    path.lineTo(l, b);
  }

  // bottomRight radius
  if (br) {
    path.arcTo(r - br, b - br, br, br, 270, 90);
  } else {
    path.lineTo(r, b);
  }

  // topRight radius
  if (tr) {
    path.arcTo(r - tr, t, tr, tr, 0, 90);
  } else {
    path.lineTo(r, t);
  }

  // path.lineTo(r, t);

  path.closeSubpath();

  return path;
}

void Rectangle::updatePolish() {
  m_box = createPath(m_border->width());
  update();
}

void Rectangle::paint(QPainter *paint) {
  paint->setRenderHint(QPainter::Antialiasing);

  const auto &border = *m_border;

  if (border.width() > 0) {
    if (border.gradient()->valid()) {
      auto gradient = border.gradient()->toQGradient(width(), height());
      paint->setPen({{*gradient}, static_cast<qreal>(border.width())});
    } else {
      paint->setPen(
          QPen(m_border->color(), static_cast<qreal>(m_border->width())));
    }
  } else {
    paint->setPen(Qt::NoPen);
  }

  if (m_gradient->valid()) {
    paint->setBrush(*m_gradient->toQGradient(width(), height()));
  } else {
    paint->setBrush(m_color);
  }

  paint->drawPath(m_box);
}

//! END: Rectangle

} // namespace Muelle