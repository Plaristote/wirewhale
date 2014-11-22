#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>
#include <QQmlContext>
#include <QQmlProperty>
#include "qpackettable.h"

int main(int argc, char *argv[])
{
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;

  engine.load(QUrl("qml/Wirewhale/main.qml"));
  if (engine.rootObjects().size() > 0)
  {
    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));
    QPacketTable* packet_table = new QPacketTable(window);

    QPacket packet;
    packet.alertLevel = 0;
    packet.number = 1;
    packet.time = 3.174783000;
    packet.source = "208.117.250.248";
    packet.destination = "10.42.43.11";
    packet.protocol = "TCP";
    packet.length = 1494;
    packet.information = "[TCP segment of a reassembled PDU]";
    packet_table->addPackets(QVector<QPacket>() << packet);
    engine.rootContext()->setContextProperty("packetLogModel", packet_table);
    QQmlProperty::write(window->findChild<QObject*>("packetLog"),
                        "model",
                        engine.rootContext()->contextProperty("packetLogModel"));
    window->show();
    packet.number = 2;
    packet_table->addPackets(QVector<QPacket>() << packet);
    return app.exec();
  }
  else
    qDebug() << "no interface to display";
  return -1;
}
