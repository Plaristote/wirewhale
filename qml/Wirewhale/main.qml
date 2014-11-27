import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

ApplicationWindow {
  title: qsTr("Wirewhale")
  width: 800
  height: 600

  Connections {
    target: packetListener
    onSnifferErrorCatched: {
      alertDialog.icon = StandardIcon.Critical;
      alertDialog.text = packetListener.lastError;
      alertDialog.open();
    }
  }

  MessageDialog {
    id: alertDialog
  }

  ColumnLayout {
    anchors.fill: parent;
    anchors.margins: 10
    spacing: 10

    Toolbar {
      id: toolbar
      Layout.fillWidth: true
    }

    PacketTable {
      id:    packetLog
      model: packetLogModel
      Layout.fillWidth:  true
      Layout.fillHeight: true
    }

    TextArea {
      Layout.fillWidth:  true
      Layout.fillHeight: true
      text: packetLogModel.getDataAt(packetLog.currentRow, "payload")
      visible: text != ""
      readOnly: true
      wrapMode: TextEdit.Wrap
      font.family: "Courier"
    }
  }
}
