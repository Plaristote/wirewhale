#include <QtGui/QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QDebug>
#include <QQmlContext>
#include <QQmlProperty>
#include "wirewhale.h"
#include <iostream>

int main(int argc, char *argv[])
{
  QGuiApplication       app(argc, argv);
  QQmlApplicationEngine engine;
  Wirewhale*            wirewhale = new Wirewhale();

  wirewhale->setQmlContext(engine.rootContext());
  engine.rootContext()->setContextProperty("wirewhale", wirewhale);
  engine.load(QUrl("qml/Wirewhale/main.qml"));
  if (engine.rootObjects().size() > 0)
  {
    QQuickWindow* window = qobject_cast<QQuickWindow*>(engine.rootObjects().value(0));

    wirewhale->setParent(window);
    window->show();
    return app.exec();
  }
  else
    qDebug() << "no interface to display";
  return -1;
}
