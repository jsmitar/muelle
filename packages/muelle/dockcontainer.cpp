#include "dockcontainer.hpp"

namespace Muelle {

Container::Container(QObject *parent)
    : QObject(parent), mEngine(new EnhancedQmlEngine(this)),
      mTheme(new Plasma::Theme(this)),
      mConfig(KSharedConfig::openConfig(QStringLiteral("muellerc"))) {

  auto debounce = new DebounceSignal(250, this);
  debounce->add(qGuiApp, &QGuiApplication::screenAdded);
  debounce->add(qGuiApp, &QGuiApplication::screenRemoved);
  debounce->add(qGuiApp, &QGuiApplication::primaryScreenChanged);
  debounce->add(this, &Container::nativeScreensChanged);
  debounce->callOnTrigger(this, &Container::updateScreens);
  updateScreens();

  connect(this, &Container::screensChanged, this, &Container::reloadViews);
  qGuiApp->installNativeEventFilter(this);

  Muelle::registerExtensions(*mEngine);
  mEngine->rootContext()->setContextProperty(QStringLiteral("$container"),
                                             this);
}

void Container::updateScreens() {
  mScreens.clear();
  foreach (auto screen, qGuiApp->screens()) {
    mScreens[screen->name()] = QVariant::fromValue(screen);
  }
  emit screensChanged();
}

void Container::loadConfiguration() {
  auto groups = mConfig->group("shell").groupList();

  if (groups.isEmpty()) {
    auto uuid = createDefaultView();
    loadView(uuid);
    mConfig->sync();
  } else {
    foreach (auto uuid, groups) { loadView(uuid); }
  }
}

bool Container::loadView(const UUID &uuid) {
  if (mViews.value(uuid, nullptr))
    return false;

  auto group = mConfig->group("shell");
  if (!group.hasGroup(uuid))
    return false;

  auto shellGroup = group.group(uuid);

  auto screen = findScreen(shellGroup.readEntry("screen", "Primary"));

  if (!screen)
    return false;

  auto view = mViews[uuid] = new View(mEngine, shellGroup);

  connect(view, &View::loaded, [&, uuid, screen] {
    if (auto view = mViews[uuid]; view)
      view->setScreen(screen);
  });
  view->load();

  return true;
}

void Container::unloadView(const UUID &uuid) {
  if (mViews.value(uuid, nullptr)) {
    std::exchange(mViews[uuid], nullptr)->unload();
    qInfo() << "unload" << uuid << mViews[uuid];
  }
}

void Container::reloadViews() {
  qDebug() << "reload views";

  auto shellGroup = mConfig->group("shell");

  foreach (const auto uuid, shellGroup.groupList()) {
    const auto screenName = shellGroup.group(uuid).readEntry("screen");

    if (findScreen(screenName)) {
      loadView(uuid);
    } else {
      unloadView(uuid);
    }
  }
}

UUID Container::createDefaultView() {
  const auto screens = QApplication::screens();

  auto uuid = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
  mConfig->group("shell").group(uuid).writeEntry(
      "screen", QApplication::primaryScreen()->name());
  return uuid;
}

QScreen *Container::findScreen(const QString &name) const {
  if (name == "Primary") {
    return QApplication::primaryScreen();
  }

  const auto screens = QApplication::screens();

  auto screen =
      std::find_if(std::cbegin(screens), std::cend(screens),
                   [&](auto screen) { return name == screen->name(); });

  return screen != std::cend(screens) ? *screen : nullptr;
}

Plasma::Theme *Container::theme() { return mTheme; }

Container *Container::instance() noexcept {
  static auto container{new Container()};

  return container;
}

QMap<QString, QVariant> Container::screensVariant() const noexcept {
  return mScreens;
}

bool Container::nativeEventFilter(const QByteArray &eventType, void *message,
                                  long *) {

  // see https://bugs.kde.org/show_bug.cgi?id=373880
  if (eventType != "xcb_generic_event_t") {
    return false;
  }

  auto *ev = static_cast<xcb_generic_event_t *>(message);

  const auto responseType = XCB_EVENT_RESPONSE_TYPE(ev);

  const xcb_query_extension_reply_t *reply =
      xcb_get_extension_data(QX11Info::connection(), &xcb_randr_id);

  if (responseType == reply->first_event + XCB_RANDR_SCREEN_CHANGE_NOTIFY) {
    emit nativeScreensChanged();
  }

  return false;
}

} // namespace Muelle
