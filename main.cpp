#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>
#include <QQmlContext>
#include <QQmlProperty>
#include "qpackettable.h"
#include "qpacketlistener.h"
#include "networkinterfacelist.h"
#include <iostream>

int main(int argc, char *argv[])
{
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;
  QPacketTable*         packetTable    = new QPacketTable();
  QPacketListener*      packetListener = new QPacketListener(packetTable);
  NetworkInterfaceList  interface_list;

  std::cout << "Launching application" << std::endl;
  engine.rootContext()->setContextProperty("packetListener", packetListener);
  engine.rootContext()->setContextProperty("packetLogModel", packetTable);
  engine.rootContext()->setContextProperty("interfaceListModel", interface_list);
  engine.load(QUrl("qml/Wirewhale/main.qml"));
  if (engine.rootObjects().size() > 0)
  {
    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));

    packetTable->setParent(window);
    packetListener->setParent(window);
    window->show();
    packetListener->setProperty("interface", "enp0s25");
    return app.exec();
  }
  else
    qDebug() << "no interface to display";
  return -1;
}
