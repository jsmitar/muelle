#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP

#include "../helpers.hpp"

#include <tuple>

#include <QBrush>
#include <QColor>
#include <QGradient>
#include <QJSEngine>
#include <QJSValue>
#include <QLinearGradient>
#include <QObject>
#include <QPainter>
#include <QPainterPath>
#include <QPair>
#include <QPen>
#include <QQmlEngine>
#include <QQmlListProperty>
#include <QQuickPaintedItem>
#include <QRegion>
#include <QVariant>
#include <QVector>
#include <array>

#include <memory>
enum Corner { BottomRight, BottomLeft, TopLeft, TopRight };
/**
 * @brief description: https://mathworld.wolfram.com/Superellipse.html
 */
template <Corner C> struct Superellipse {
public:
  /** * @param curve common values: 4:squircle, 2:ellipse, 1:diamond
   */

  using Path = std::array<QPointF, 91>;

  const Path &operator()(qreal curve, qreal size) {
    curve = clamp(0.1, curve, 100.0);
    size = max(1.0, size);

    if (m_curve != curve || m_size != size) {
      m_curve = curve;
      m_size = size;

      constexpr uint deg{C * 90};

      for (uint step{0}; step <= 90; ++step)
        m_path[step] = superellipse(deg + step);
    }
    return m_path;
  }

  QPointF superellipse(qreal deg) {
    constexpr auto radian{M_PI * 2 / 360};
    const auto t{radian * deg};

    const auto cos_t = std::cos(t);
    const auto sin_t = std::sin(t);
    const auto curve = 2 / m_curve;

    return {sign(cos_t) * m_size * std::pow(std::abs(cos_t), curve),
            sign(sin_t) * m_size * std::pow(std::abs(sin_t), curve)};
  }

private:
  qreal m_size{-1};
  qreal m_curve{-1};
  Path m_path;
};

namespace Muelle {

class Gradient : public QObject {
  Q_OBJECT
  Q_PROPERTY(QVariant stops READ stops WRITE setStops NOTIFY changed)
  Q_PROPERTY(int degrees READ degrees WRITE setDegrees NOTIFY changed)
public:
  Gradient(QObject *parent = nullptr) noexcept;
  ~Gradient() override{};
  /**
   * degrees: follow clockwise starting at 12:00 as 0deg and 6:00 as
   * 180deg
   * */
  void setDegrees(int degrees);
  int degrees() const noexcept;

  void setStops(QVariant value) noexcept;
  QVariant stops() const noexcept;

  QLinearGradient *toQGradient(qreal width, qreal height) noexcept;

  inline bool valid() const noexcept { return !m_stops.isUndefined(); };

signals:
  void changed();

private:
  int m_degrees{0};
  QJSValue m_stops;
  std::unique_ptr<QLinearGradient> m_gradient;
};

class Border : public QObject {
  Q_OBJECT
  Q_PROPERTY(qreal width READ width WRITE setWidth NOTIFY widthChanged)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(Muelle::Gradient *gradient READ gradient)

public:
  Border(QObject *parent = nullptr) noexcept
      : QObject(parent), m_gradient(new Gradient(this)) {}
  ~Border() noexcept override{};

  inline void setWidth(qreal width) noexcept {
    if (m_width != width) {
      m_width = max(0, width);
      emit widthChanged();
    }
  }

  inline qreal width() const noexcept { return m_width; }

  inline QColor color() const noexcept { return m_color; }

  inline void setColor(const QColor &color) noexcept {
    if (m_color != color) {
      m_color = color;
      emit colorChanged();
    }
  };

  inline Gradient *gradient() const noexcept { return m_gradient; }

signals:
  void widthChanged();
  void colorChanged();

private:
  qreal m_width{0};
  QColor m_color;
  Gradient *m_gradient;
};

class Radius : public QObject {
  Q_OBJECT
  Q_PROPERTY(qreal topLeft READ topLeft WRITE setTopLeft NOTIFY changed)
  Q_PROPERTY(qreal topRight READ topRight WRITE setTopRight NOTIFY changed)
  Q_PROPERTY(
      qreal bottomLeft READ bottomLeft WRITE setBottomLeft NOTIFY changed)
  Q_PROPERTY(
      qreal bottomRight READ bottomRight WRITE setBottomRight NOTIFY changed)

public:
  Radius(QObject *parent = nullptr) noexcept : QObject(parent) {}
  ~Radius() noexcept override{};

  inline void setTopLeft(qreal radius) noexcept {
    if (mTopLeft != radius) {
      mTopLeft = max(0, radius);
      emit changed();
    }
  }

  inline qreal topLeft() const noexcept { return mTopLeft; }

  inline void setTopRight(qreal radius) noexcept {
    if (mTopRight != radius) {
      mTopRight = max(0, radius);
      emit changed();
    }
  }

  inline qreal topRight() const noexcept { return mTopRight; }

  inline void setBottomLeft(qreal radius) {
    if (mBottomLeft != radius) {
      mBottomLeft = max(0, radius);
      emit changed();
    }
  }

  inline qreal bottomLeft() const noexcept { return mBottomLeft; }

  inline void setBottomRight(qreal radius) noexcept {
    if (mBottomRight != radius) {
      mBottomRight = max(0, radius);
      emit changed();
    }
  }

  inline qreal bottomRight() const noexcept { return mBottomRight; }

signals:
  void changed();

private:
  qreal mTopLeft{0};
  qreal mTopRight{0};
  qreal mBottomLeft{0};
  qreal mBottomRight{0};
};

class Rectangle : public QQuickPaintedItem {
  Q_OBJECT
  Q_PROPERTY(Muelle::Radius *radius READ radius)
  Q_PROPERTY(Muelle::Border *border READ border)
  Q_PROPERTY(Muelle::Border *outline READ outline)
  Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
  Q_PROPERTY(Muelle::Gradient *gradient READ gradient)
  Q_PROPERTY(QRegion mask READ mask NOTIFY maskChanged)
  Q_PROPERTY(bool maskEnabled READ maskEnabled WRITE setMaskEnabled NOTIFY
                 maskEnabledChanged)

public:
  Rectangle(QQuickItem *parent = 0) noexcept;
  ~Rectangle() noexcept;

  Radius *radius() const noexcept;

  Border *border() const noexcept;

  Border *outline() const noexcept;

  QColor color() const noexcept;
  void setColor(const QColor &color) noexcept;

  Gradient *gradient() const noexcept;

  bool maskEnabled() const;
  void setMaskEnabled(bool enabled);

  QRegion mask() const;

  void createPath(qreal border);

  void updateMask();

  void update();
  void updatePolish() override;

  void paint(QPainter *painter) override;

signals:
  void colorChanged();
  void maskChanged();
  void maskEnabledChanged();
  void didPaint();

private:
  Radius *m_radius;
  Border *m_outline;
  Border *m_border;
  Gradient *m_gradient;
  QColor m_color;
  QPainterPath m_box;
  QRegion m_region;
  bool m_maskEnabled{false};

  Superellipse<Corner::BottomRight> m_br;
  Superellipse<Corner::BottomLeft> m_bl;
  Superellipse<Corner::TopLeft> m_tl;
  Superellipse<Corner::TopRight> m_tr;
};

} // namespace Muelle

inline void qmlRegisterRectangle() {
  qmlRegisterType<Muelle::Rectangle>("org.muelle.extra", 1, 0, "Rectangle");
  qmlRegisterType<Muelle::Border>("org.muelle.extra", 1, 0, "Border");
  qmlRegisterType<Muelle::Radius>("org.muelle.extra", 1, 0, "Radius");
  qmlRegisterType<Muelle::Gradient>("org.muelle.extra", 1, 0, "Gradient");
  qmlRegisterUncreatableMetaObject(QGradient::staticMetaObject, "io.qt", 1, 0,
                                   "QGradient", "Access to enums only");
}
#endif // RECTANGLE_HPP