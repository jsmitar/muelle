#ifndef VIEWCONTRAST_HPP
#define VIEWCONTRAST_HPP

#include "dockcontainer.hpp"
#include "docktypes.hpp"
#include "libs/debouncesignal.hpp"
#include <KWindowEffects>
#include <Plasma/Theme>
#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QRegion>
#include <QWindow>

namespace Muelle {
class ViewContrast : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QWindow *view READ view WRITE setView NOTIFY viewChanged)
  Q_PROPERTY(QRegion mask READ mask WRITE setMask NOTIFY maskChanged)
  Q_PROPERTY(QPoint position READ position WRITE setPosition NOTIFY maskChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(bool blurEnabled READ blurEnabled WRITE setBlurEnabled NOTIFY
                 blurEnabledChanged)
  Q_PROPERTY(bool contrastEnabled READ contrastEnabled WRITE setContrastEnabled
                 NOTIFY contrastEnabledChanged)
  Q_PROPERTY(
      qreal contrast READ contrast WRITE setContrast NOTIFY contrastChanged)
  Q_PROPERTY(
      qreal intensity READ intensity WRITE setIntensity NOTIFY intensityChanged)
  Q_PROPERTY(qreal saturation READ saturation WRITE setSaturation NOTIFY
                 saturationChanged)
  Q_PROPERTY(bool usePlasmaTheme READ usePlasmaTheme WRITE setUsePlasmaTheme
                 NOTIFY usePlasmaThemeChanged)

public:
  ViewContrast(QObject *parent = nullptr) noexcept;
  ~ViewContrast() noexcept override;

  void classBegin() override;
  void componentComplete() override;

  void setView(QWindow *view) noexcept;
  QWindow *view() const noexcept;

  void setMask(const QRegion &region) noexcept;
  QRegion mask() const noexcept;

  void setPosition(const QPoint &point) noexcept;
  QPoint position() const noexcept;

  void setEnabled(bool enabled) noexcept;
  bool enabled() const noexcept;

  void setBlurEnabled(bool enabled) noexcept;
  bool blurEnabled() const noexcept;

  void setContrastEnabled(bool enabled) noexcept;
  bool contrastEnabled() const noexcept;

  void setContrast(qreal value) noexcept;
  qreal contrast() const noexcept;

  void setIntensity(qreal value) noexcept;
  qreal intensity() const noexcept;

  void setSaturation(qreal value) noexcept;
  qreal saturation() const noexcept;

  void setUsePlasmaTheme(bool value) noexcept;
  bool usePlasmaTheme() const noexcept;

public:
  void updateContrastValues() noexcept;
  void update() noexcept;

signals:
  void viewChanged();
  void maskChanged();
  void enabledChanged();
  void blurEnabledChanged();
  void contrastEnabledChanged();
  void contrastChanged();
  void intensityChanged();
  void saturationChanged();
  void usePlasmaThemeChanged();

private:
  bool mEnabled{false};
  bool mBlurEnabled{false};
  bool mContrastEnabled{false};
  bool mUsePlasmaTheme{false};

  qreal mContrast{0};
  qreal mIntensity{0};
  qreal mSaturation{0};

  QRegion mMask;
  QPoint mPosition;
  QWindow *mView{nullptr};
};
} // namespace Muelle

inline void qmlRegisterViewContrast() {
  qmlRegisterType<Muelle::ViewContrast>("org.muelle.extra", 1, 0,
                                        "ViewContrast");
}
#endif // VIEWCONTRAST_HPP
