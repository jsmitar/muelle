#include "configuration.hpp"

namespace Muelle {

Configuration::Configuration(QObject *parent, KConfigGroup &config)
    : PropertyMap(this, parent), mConfig(config) {}

void Configuration::save() {
  const auto keys = this->keys();
  foreach (auto k, keys) { mConfig.writeEntry(k, value(k)); }
  mConfig.sync();
}

} // namespace Muelle
