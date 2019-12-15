#include "qobjectpropertyvaluesource.hpp"

QObjectPropertyValueSource::QObjectPropertyValueSource(QObject *parent)
  : QObject(parent)
{}

void QObjectPropertyValueSource::classBegin() {}

void QObjectPropertyValueSource::setTarget(const QQmlProperty &prop)
{
  const auto signalIndex = mTargetProperty.property().notifySignal().methodIndex();
  const auto signalMethod = mTargetProperty.object()->metaObject()->method(signalIndex);

  QObject::disconnect(mTargetProperty.object(),
                      signalMethod,
                      this,
                      QMetaMethod::fromSignal(&QObjectPropertyValueSource::targetValueChanged));

  mTargetProperty = prop;
  prop.connectNotifySignal(this,
                           QMetaMethod::fromSignal(&QObjectPropertyValueSource::targetValueChanged)
                             .methodIndex());
  emit targetValueChanged();
}

QQmlProperty QObjectPropertyValueSource::target() const
{
  return mTargetProperty;
}

void QObjectPropertyValueSource::setInitial(const QVariant &initial)
{
  mInitial = initial;
}

QVariant QObjectPropertyValueSource::initial() const
{
  return mInitial;
}

QVariant QObjectPropertyValueSource::value() const
{
  return mTargetProperty.read();
}

void QObjectPropertyValueSource::componentComplete()
{
  mTargetProperty.write(mInitial);
}
