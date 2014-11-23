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

  std::cout << "Launching application" << std::endl;
  engine.load(QUrl("qml/Wirewhale/main.qml"));
  if (engine.rootObjects().size() > 0)
  {
    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));
    NetworkInterfaceList interface_list;
    QPacketTable*        packetTable    = new QPacketTable(window);
    QPacketListener*     packetListener = new QPacketListener(packetTable, window);

    engine.rootContext()->setContextProperty("packetListener", packetListener);
    engine.rootContext()->setContextProperty("packetLogModel", packetTable);
    engine.rootContext()->setContextProperty("interfaceListModel", interface_list);

    window->show();
    packetListener->setProperty("interface", "enp0s25");
    return app.exec();
  }
  else
    qDebug() << "no interface to display";
  return -1;
}
