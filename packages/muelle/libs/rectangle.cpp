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
      m_outline(new Border(this)), m_border(new Border(this)),
      m_gradient(new Gradient(this)) {

  setAntialiasing(true);

  m_box.reserve(365);

  // UPDATE
  connect(m_outline, &Border::colorChanged, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);
  connect(m_outline->gradient(), &Gradient::changed, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);

  connect(m_border, &Border::colorChanged, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);
  connect(m_border->gradient(), &Gradient::changed, this,
          qOverload<>(&Rectangle::update), Qt::DirectConnection);

  // UPDATE POLISH
  connect(m_gradient, &Gradient::changed, this, qOverload<>(&Rectangle::update),
          Qt::DirectConnection);

  connect(m_radius, &Radius::changed, this, &Rectangle::updatePolish,
          Qt::DirectConnection);

  connect(m_outline, &Border::widthChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
  connect(m_border, &Border::widthChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);

  connect(this, &Rectangle::widthChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
  connect(this, &Rectangle::heightChanged, this, &Rectangle::updatePolish,
          Qt::DirectConnection);
}

Rectangle::~Rectangle() noexcept {};

Radius *Rectangle::radius() const noexcept { return m_radius; }

Border *Rectangle::border() const noexcept { return m_border; }

Border *Rectangle::outline() const noexcept { return m_outline; }

void Rectangle::setColor(const QColor &color) noexcept {
  m_color = color;
  emit colorChanged();
  update();
}
QColor Rectangle::color() const noexcept { return m_color; };

Gradient *Rectangle::gradient() const noexcept { return m_gradient; };

bool Rectangle::maskEnabled() const { return m_maskEnabled; }
void Rectangle::setMaskEnabled(bool enabled) {
  if (m_maskEnabled != enabled) {
    m_maskEnabled = enabled;
    emit maskEnabledChanged();
  }
}

QRegion Rectangle::mask() const { return m_region; }

void Rectangle::createPath() {
  const auto border{m_border->width() + m_outline->width()};
  const auto w{width() - border * 2};
  const auto h{height() - border * 2};

  const auto corner_size{min(w, h) / 2.0};

  const auto br_superellipse{m_br(m_radius->bottomRight(), corner_size)};
  const auto bl_superellipse{m_bl(m_radius->bottomLeft(), corner_size)};
  const auto tl_superellipse{m_tl(m_radius->topLeft(), corner_size)};
  const auto tr_superellipse{m_tr(m_radius->topRight(), corner_size)};

  m_box.clear();
  m_box.moveTo({border + w, border + h - corner_size});

  const auto addSuperellipse = [&](const auto &superellipse,
                                   const QPointF &start) {
    for (const auto &point : superellipse)
      m_box.lineTo(start + point);
  };

  addSuperellipse(br_superellipse,
                  {border + w - corner_size, border + h - corner_size});
  addSuperellipse(bl_superellipse,
                  {border + corner_size, border + h - corner_size});
  addSuperellipse(tl_superellipse,
                  {corner_size + border, border + corner_size});
  addSuperellipse(tr_superellipse,
                  {border + w - corner_size, border + corner_size});

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
  createPath();
  update();
  if (m_maskEnabled) {
    updateMask();
  }
}

void Rectangle::paint(QPainter *paint) {
  paint->setCompositionMode(QPainter::CompositionMode_Source);
  paint->setBackgroundMode(Qt::BGMode::TransparentMode);
  paint->setPen(Qt::NoPen);
  paint->setBrush(Qt::NoBrush);

  const auto &outline = *m_outline;
  const auto &border = *m_border;

  // OUTLINE
  if (outline.width() > 0) {
    if (outline.gradient()->valid()) {
      auto gradient = outline.gradient()->toQGradient(width(), height());
      const QPen pen{{*gradient}, (outline.width() + border.width()) * 2};
      paint->strokePath(m_box, pen);
    } else {
      const QPen pen{outline.color(), (outline.width() + border.width()) * 2};
      paint->strokePath(m_box, pen);
    }
  }

  // BORDER
  if (border.width() > 0) {
    if (border.gradient()->valid()) {
      auto gradient = border.gradient()->toQGradient(width(), height());
      const QPen pen{{*gradient}, border.width() * 2};
      paint->strokePath(m_box, pen);
    } else {
      const QPen pen{border.color(), border.width() * 2};
      paint->strokePath(m_box, pen);
    }
  }

  // BACKGROUND
  if (m_gradient->valid()) {
    paint->fillPath(m_box, *m_gradient->toQGradient(width(), height()));
  } else {
    paint->fillPath(m_box, m_color);
  }

  emit didPaint();
}

//! END: Rectangle

} // namespace Muelle