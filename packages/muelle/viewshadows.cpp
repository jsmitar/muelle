#include "viewshadows.hpp"
#include <QDebug>

namespace Muelle {

ViewShadows::ViewShadows(QObject *parent) noexcept
    : QObject(parent), mShadow(new KWindowShadow(this)) {}

ViewShadows::~ViewShadows() { mShadow->destroy(); }

void ViewShadows::classBegin() {
  if (!ViewShadows::mSvg) {
    mSvg = new Plasma::Svg(QApplication::instance());
    mSvg->setImagePath("widgets/panel-background");

    const auto addTile = [&](auto elementId) {
      KWindowShadowTile::Ptr tile{new KWindowShadowTile};
      tile->setImage(mSvg->image(mSvg->elementSize(elementId), elementId));
      mTile[elementId] = tile;
    };

    const auto addHint = [&](auto hintId, bool useWidth) {
      if (auto size = mSvg->elementSize(hintId); useWidth)
        mHint[hintId] = size.width();
      else
        mHint[hintId] = size.height();
    };

    addTile("shadow-top");
    addTile("shadow-right");
    addTile("shadow-bottom");
    addTile("shadow-left");
    addTile("shadow-topleft");
    addTile("shadow-topright");
    addTile("shadow-bottomleft");
    addTile("shadow-bottomright");

    addHint("shadow-hint-top-margin", false);
    addHint("shadow-hint-right-margin", true);
    addHint("shadow-hint-bottom-margin", false);
    addHint("shadow-hint-left-margin", true);
  }
}

void ViewShadows::componentComplete() { configureTiles(); }

void ViewShadows::setView(QWindow *view) {
  if (view != mView) {
    mView = view;
    emit viewChanged();
    configureTiles();
  }
}

QWindow *ViewShadows::view() const { return mView; }

void ViewShadows::setGeometry(const QRect &rect) {
  if (rect != mRect) {
    mRect = rect;
    emit geometryChanged();
    configureTiles();
  }
}

QRect ViewShadows::geometry() const { return mRect; }

void ViewShadows::setEdges(uint edges) {
  if (edges != mEdges) {
    mEdges = {edges};
    emit edgesChanged();
    configureTiles();
  }
}

uint ViewShadows::edges() const { return mEdges; }

void ViewShadows::setEnabled(bool enabled) {
  if (enabled != mShadow->isCreated()) {
    configureTiles();
    emit enabledChanged();
  }
}

bool ViewShadows::enabled() const { return mShadow->isCreated(); }

void ViewShadows::configureTiles() {
  mShadow->destroy();

  if (!mView)
    return;

  mShadow->setWindow(mView);

  using Edge = Muelle::Types::Edge;

  if (mEdges & Edge::Top)
    mShadow->setTopTile(mTile["shadow-top"]);

  if (mEdges & Edge::Right)
    mShadow->setRightTile(mTile["shadow-right"]);

  if (mEdges & Edge::Bottom)
    mShadow->setBottomTile(mTile["shadow-bottom"]);

  if (mEdges & Edge::Left)
    mShadow->setLeftTile(mTile["shadow-left"]);

  if (mEdges & Edge::Top && mEdges & Edge::Left)
    mShadow->setTopLeftTile(mTile["shadow-topleft"]);

  if (mEdges & Edge::Top && mEdges & Edge::Right)
    mShadow->setTopRightTile(mTile["shadow-topright"]);

  if (mEdges & Edge::Bottom && mEdges & Edge::Left)
    mShadow->setBottomLeftTile(mTile["shadow-bottomleft"]);

  if (mEdges & Edge::Bottom && mEdges & Edge::Right)
    mShadow->setBottomRightTile(mTile["shadow-bottomright"]);

  const auto size = mView->size();
  mShadow->setPadding({
      /* left   */
      -mRect.left() + mHint["shadow-hint-left-margin"],
      /* top    */
      -mRect.top() + mHint["shadow-hint-top-margin"],
      /* right  */
      -(size.width() - mRect.right() - 1) + mHint["shadow-hint-right-margin"],
      /* bottom */
      -(size.height() - mRect.bottom() - 1) +
          mHint["shadow-hint-bottom-margin"],
  });
  mShadow->create();
}

QMap<QString, KWindowShadowTile::Ptr> ViewShadows::mTile;
QMap<QString, int> ViewShadows::mHint;
Plasma::Svg *ViewShadows::mSvg{nullptr};

} // namespace Muelle
