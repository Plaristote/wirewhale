#ifndef WIREWHALE_H
#define WIREWHALE_H

#include <QObject>
#include <QStringList>
#include <QQmlContext>
#include "qpacketlistener.h"
#include "qpackettable.h"

class Wirewhale : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPacketTable*    packetTable    READ getPacketTable)
    Q_PROPERTY(QPacketListener* packetListener READ getPacketListener)
    Q_PROPERTY(QStringList      interfaceList  READ getInterfaceList NOTIFY interfaceListUpdated)
    Q_PROPERTY(QStringList      profileList    READ getProfileList   NOTIFY profileListUpdated)

public:
    explicit Wirewhale(QObject *parent = 0);

    QStringList      getProfileList()    const;
    QStringList      getInterfaceList()  const;
    QPacketTable*    getPacketTable()    const { return (packetTable);    }
    QPacketListener* getPacketListener() const { return (packetListener); }

    void setQmlContext(QQmlContext*);

signals:
    void profileListUpdated();
    void interfaceListUpdated();

public slots:

private:
    QPacketTable*        packetTable;
    QPacketListener*     packetListener;
};

#endif // WIREWHALE_H
