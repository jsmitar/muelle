#include "viewcontrast.hpp"
#include <QDebug>

namespace Muelle {
ViewContrast::ViewContrast(QObject *parent) noexcept : QObject(parent) {}

ViewContrast::~ViewContrast() noexcept {}

void ViewContrast::classBegin() {}

void ViewContrast::componentComplete() {}

void ViewContrast::setView(QWindow *view) noexcept {
  mView = view;
  emit viewChanged();
}

QWindow *ViewContrast::view() const noexcept { return mView; }

void ViewContrast::setMask(const QRegion &region) noexcept {
  mMask = region;
  emit maskChanged();
}

QRegion ViewContrast::mask() const noexcept { return mMask; }

void ViewContrast::setPosition(const QPoint &point) noexcept {
  if (mPosition == point)
    return;
  mPosition = point;
  emit maskChanged();
}

QPoint ViewContrast::position() const noexcept { return mPosition; }

void ViewContrast::setEnabled(bool enabled) noexcept {
  mEnabled = enabled;

  if (mEnabled) {
    connect(this, &ViewContrast::maskChanged, this, &ViewContrast::update);
  } else {
    disconnect(this, &ViewContrast::maskChanged, this, &ViewContrast::update);
  }
  emit enabledChanged();
}

bool ViewContrast::enabled() const noexcept { return mEnabled; }

void ViewContrast::setBlurEnabled(bool enabled) noexcept {
  mBlurEnabled = enabled;
  emit blurEnabledChanged();
  update();
}

bool ViewContrast::blurEnabled() const noexcept { return mBlurEnabled; }

void ViewContrast::setContrastEnabled(bool enabled) noexcept {
  mContrastEnabled = enabled;
  emit contrastEnabledChanged();
  update();
}

bool ViewContrast::contrastEnabled() const noexcept { return mContrastEnabled; }

void ViewContrast::setContrast(qreal value) noexcept {
  mContrast = value;
  emit contrastChanged();
  update();
}

qreal ViewContrast::contrast() const noexcept { return mContrast; }

void ViewContrast::setIntensity(qreal value) noexcept {
  mIntensity = value;
  emit intensityChanged();
  update();
}

qreal ViewContrast::intensity() const noexcept { return mIntensity; }

void ViewContrast::setSaturation(qreal value) noexcept {
  mSaturation = value;
  emit saturationChanged();
  update();
}

qreal ViewContrast::saturation() const noexcept { return mSaturation; }

void ViewContrast::setUsePlasmaTheme(bool value) noexcept {
  if (mUsePlasmaTheme == value)
    return;

  const auto theme{Container::instance()->theme()};
  if (mUsePlasmaTheme) {
    connect(theme, &Plasma::Theme::themeChanged, this,
            &ViewContrast::updateContrastValues);
  } else {
    disconnect(theme, &Plasma::Theme::themeChanged, this,
               &ViewContrast::updateContrastValues);
  }

  emit usePlasmaThemeChanged();
  updateContrastValues();
}

bool ViewContrast::usePlasmaTheme() const noexcept { return mUsePlasmaTheme; };

void ViewContrast::update() noexcept {
  if (!mView)
    return;

  const auto winId{mView->winId()};
  if (mEnabled) {
    const auto mask{mMask.translated(mPosition)};

    KWindowEffects::enableBlurBehind(winId, mBlurEnabled, mask);
    KWindowEffects::enableBackgroundContrast(winId, mContrastEnabled, mContrast,
                                             mIntensity, mSaturation, mask);
  } else {
    KWindowEffects::enableBlurBehind(winId, false);
    KWindowEffects::enableBackgroundContrast(winId, false);
  }
}

void ViewContrast::updateContrastValues() noexcept {
  const auto theme{Container::instance()->theme()};

  mBlurEnabled = theme->blurBehindEnabled();
  mContrastEnabled = theme->backgroundContrastEnabled();
  mContrast = theme->backgroundContrast();
  mIntensity = theme->backgroundIntensity();
  mSaturation = theme->backgroundSaturation();

  update();
}

} // namespace Muelle