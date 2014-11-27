import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.0

ApplicationWindow {
  id: window
  title: qsTr("Wirewhale")
  width: 800
  height: 600

  Settings {
    property alias x:                 window.x
    property alias y:                 window.y
    property alias width:             window.width
    property alias height:            window.height
  }

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
    icon: StandardIcon.Critical
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
