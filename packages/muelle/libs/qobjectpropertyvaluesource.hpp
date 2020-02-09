#ifndef QOBJECTPROPERTYVALUESOURCE_HPP
#define QOBJECTPROPERTYVALUESOURCE_HPP

#include <QMetaMethod>
#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QQmlProperty>
#include <QQmlPropertyValueSource>
#include <QVariant>
#include <qglobal.h>

class QObjectPropertyValueSource : public QObject,
                                   public QQmlPropertyValueSource,
                                   public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlPropertyValueSource)
  Q_INTERFACES(QQmlParserStatus)

  Q_PROPERTY(QVariant value READ value NOTIFY value NOTIFY targetValueChanged)
  Q_PROPERTY(
      QVariant initial READ initial WRITE setInitial NOTIFY initialChanged)

public:
  explicit QObjectPropertyValueSource(QObject *parent = nullptr);
  ~QObjectPropertyValueSource() override = default;

  void classBegin() override;
  void componentComplete() override;

  void setTarget(const QQmlProperty &prop) override;
  QQmlProperty target() const;

  void setInitial(const QVariant &initial);
  QVariant initial() const;

  QVariant value() const;

signals:
  void targetValueChanged();
  void initialChanged();

private:
  QQmlProperty mTargetProperty;
  QVariant mInitial;
  QVariant mValue;
};

inline void qmlRegisterQObjectPropertyValueSource() {
  qmlRegisterType<QObjectPropertyValueSource>("org.muelle.extra", 1, 0,
                                              "QObjectPropertyValueSource");
}

#endif // QOBJECTPROPERTYVALUESOURCE_HPP
