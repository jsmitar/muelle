#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include "./libs/propertymap.hpp"

#include <KConfig>
#include <KConfigGroup>
#include <QObject>
#include <QQmlParserStatus>
#include <map>

namespace Muelle {
class Configuration : public PropertyMap {
  Q_OBJECT

public:
  explicit Configuration(QObject *parent, KConfigGroup &config);

  void init();
  void setDefaults();

  Q_INVOKABLE void save();

private:
  KConfigGroup mConfig;
};
} // namespace Muelle

#endif // CONFIGURATION_HPP
