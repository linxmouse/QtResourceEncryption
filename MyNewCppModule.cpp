#include "MyNewCppModule.h"
#include <QDebug>

MyNewCppModule::MyNewCppModule(QObject *parent) : QObject(parent) {}

MyNewCppModule::~MyNewCppModule() {}

void MyNewCppModule::printHello() { qDebug() << "Hello from C++!"; }