#include "configuration.hpp"

namespace Muelle {

Configuration::Configuration(QObject *parent, KConfigGroup &config)
    : PropertyMap(this, parent), mConfig(config) {
  init();
}

Configuration::~Configuration() noexcept { save(); }

void Configuration::init() {
  foreach (auto key, mConfig.keyList()) { insert(key, mConfig.readEntry(key)); }

  connect(this, &QQmlPropertyMap::valueChanged,
          [&](auto &key, auto &value) { mConfig.writeEntry(key, value); });
}

void Configuration::save() { mConfig.sync(); }

} // namespace Muelle
