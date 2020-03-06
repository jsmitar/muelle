#include "viewshadows.hpp"

namespace Muelle {

ViewShadows::ViewShadows(QObject *parent) noexcept
    : QObject(parent), mSvg(new Plasma::Svg(this)),
      mShadow(new KWindowShadow(this)) {
  mSvg->setImagePath("translucent/widgets/panel-background");
}

ViewShadows::~ViewShadows() { mShadow->destroy(); }

void ViewShadows::classBegin() {}

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

  const auto tile = [&](auto elementId) -> KWindowShadowTile::Ptr {
    KWindowShadowTile::Ptr tile{new KWindowShadowTile};
    tile->setImage(mSvg->image(mSvg->elementSize(elementId), elementId));
    return tile;
  };

  mShadow->setWindow(mView);

  using Edge = Muelle::Types::Edge;

  if (mEdges & Edge::Top)
    mShadow->setTopTile(tile("shadow-top"));

  if (mEdges & Edge::Right)
    mShadow->setRightTile(tile("shadow-right"));

  if (mEdges & Edge::Bottom)
    mShadow->setBottomTile(tile("shadow-bottom"));

  if (mEdges & Edge::Left)
    mShadow->setLeftTile(tile("shadow-left"));

  if (mEdges & Edge::Top && mEdges & Edge::Left)
    mShadow->setTopLeftTile(tile("shadow-topleft"));

  if (mEdges & Edge::Top && mEdges & Edge::Right)
    mShadow->setTopRightTile(tile("shadow-topright"));

  if (mEdges & Edge::Bottom && mEdges & Edge::Left)
    mShadow->setBottomLeftTile(tile("shadow-bottomleft"));

  if (mEdges & Edge::Bottom && mEdges & Edge::Right)
    mShadow->setBottomRightTile(tile("shadow-bottomright"));

  updateShadowGeometry();
  mShadow->create();
}

void ViewShadows::updateShadowGeometry() {
  const auto size = mView->size();
  mShadow->setPadding({
      /* left   */ -mRect.left() + 6,
      /* top    */ -mRect.top() + 6,
      /* right  */ -(size.width() - mRect.right()) + 7,
      /* bottom */ -(size.height() - mRect.bottom()) + 7,
  });
}

} // namespace Muelle
