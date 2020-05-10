#include "dockcontainer.hpp"

namespace Muelle {

Container::Container(QObject *parent)
    : QObject(parent), mEngine(new EnhancedQmlEngine(this)),
      mTheme(new Plasma::Theme(this)),
      mConfig(KSharedConfig::openConfig(QStringLiteral("muellerc"))) {

  Muelle::registerExtensions(*mEngine);

  const auto updateScreens = [&] {
    mScreens.clear();
    foreach (auto screen, qGuiApp->screens()) {
      mScreens[screen->name()] = QVariant::fromValue(screen);
    }
    emit screensChanged();
  };
  updateScreens();

  connect(qGuiApp, &QGuiApplication::screenAdded, updateScreens);
  connect(qGuiApp, &QGuiApplication::screenRemoved, updateScreens);

  connect(qGuiApp, &QGuiApplication::screenAdded, this, &Container::loadViews);
  connect(qGuiApp, &QGuiApplication::screenRemoved, this,
          &Container::unloadViews);

  mEngine->rootContext()->setContextProperty(QStringLiteral("$container"),
                                             this);
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

  auto view = new View(mEngine, shellGroup);

  mViews[uuid] = view;
  view->setScreen(screen);
  view->load();
  return true;
}

void Container::unloadView(const UUID &uuid) {
  if (mViews.value(uuid, nullptr)) {
    std::exchange(mViews[uuid], nullptr)->unload();
    qInfo() << "unload" << uuid;
  }
}

void Container::loadViews(QScreen *screen) {
  auto shellGroup = mConfig->group("shell");
  auto screenName = screen->name();

  foreach (const auto uuid, shellGroup.groupList()) {
    const auto shellScreen = shellGroup.group(uuid).readEntry("screen");

    if (shellScreen == screenName) {
      loadView(uuid);
    }
  }
}

void Container::unloadViews(QScreen *screen) {
  auto shellGroup = mConfig->group("shell");
  auto screenName = screen->name();

  foreach (const auto uuid, shellGroup.groupList()) {
    const auto shellScreen = shellGroup.group(uuid).readEntry("screen");

    if (shellScreen == screenName) {
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
} // namespace Muelle
