#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

class MyNewCppModule : public QObject {
  Q_OBJECT
  QML_ELEMENT
public:
  explicit MyNewCppModule(QObject *parent = nullptr);
  ~MyNewCppModule();

  Q_INVOKABLE void printHello();
};