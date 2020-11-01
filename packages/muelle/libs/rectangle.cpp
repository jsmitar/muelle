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

void Gradient::setDegrees(int degrees) {
  auto value = clamp(0, degrees, 360);
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

  setAntialiasing(true);

  m_box.reserve(681);

  connect(m_border, &Border::colorChanged, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);
  connect(m_border->gradient(), &Gradient::changed, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);
  connect(m_gradient, &Gradient::changed, this, qOverload<>(&Rectangle::update),
          Qt::DirectConnection);
  connect(m_radius, &Radius::changed, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
  connect(m_border, &Border::widthChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
  connect(this, &Rectangle::widthChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
  connect(this, &Rectangle::heightChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
}

Rectangle::~Rectangle() noexcept {};

Radius *Rectangle::radius() noexcept { return m_radius; }

void Rectangle::setColor(const QColor &color) noexcept {
  m_color = color;
  emit colorChanged();
  update();
}

QColor Rectangle::color() const noexcept { return m_color; };

bool Rectangle::maskEnabled() const { return m_maskEnabled; }

void Rectangle::setMaskEnabled(bool enabled) {
  if (m_maskEnabled != enabled) {
    m_maskEnabled = enabled;
    emit maskEnabledChanged();
  }
}

QRegion Rectangle::mask() const { return m_region; }

void Rectangle::createPath(qreal border) {
  const qreal w = width();
  const qreal h = height();

  const auto corner_size = min(w, h) / 2;

  const auto br_radius = m_radius->bottomRight();
  const auto bl_radius = m_radius->bottomLeft();
  const auto tl_radius = m_radius->topLeft();
  const auto tr_radius = m_radius->topRight();

  const auto br = m_br(br_radius, corner_size)
                      .translated({w - corner_size, h - corner_size});

  const auto bl =
      m_bl(bl_radius, corner_size).translated({corner_size, h - corner_size});

  const auto tl =
      m_tl(tl_radius, corner_size).translated({corner_size, corner_size});

  const auto tr =
      m_tr(tr_radius, corner_size).translated({w - corner_size, corner_size});

  m_box.clear();
  m_box.connectPath(br);
  m_box.connectPath(bl);
  m_box.connectPath(tl);
  m_box.connectPath(tr);
  m_box.closeSubpath();
}

void Rectangle::updateMask() {
  QBitmap bitmap(width(), height());
  bitmap.clear();
  QPainter p(&bitmap);

  if (!p.isActive())
    return;

  p.setBackgroundMode(Qt::BGMode::TransparentMode);
  p.setPen(Qt::PenStyle::NoPen);
  p.fillRect(bitmap.rect(), Qt::transparent);

  p.setBrush(Qt::black);
  p.drawPath(m_box);

  m_region = QRegion(bitmap);

  emit maskChanged();
}

void Rectangle::update() { QQuickPaintedItem::update(); }

void Rectangle::updatePolish() {
  createPath(m_border->width());
  update();
  if (m_maskEnabled) {
    updateMask();
  }
}

void Rectangle::paint(QPainter *paint) {
  paint->setBackgroundMode(Qt::BGMode::TransparentMode);

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

  emit didPaint();
}

//! END: Rectangle

} // namespace Muelle