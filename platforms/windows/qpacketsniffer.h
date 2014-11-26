#ifndef  WINDOWS_QPACKET_SNIFFER_H
# define WINDOWS_QPACKET_SNIFFER_H

# include "../../qabstractpacketsniffer.h"

class QPacketSniffer : public QAbstractPacketSniffer
{
  Q_OBJECT

public:
  QPacketSniffer(const QString& interface_name, QObject* parent);
  ~QPacketSniffer();

  void run();
  void stop();
  void wait();

private:
};

#endif
