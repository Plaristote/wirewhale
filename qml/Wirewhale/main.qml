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

    RowLayout {
      id: toolbar
      spacing: 6
      Layout.fillWidth: true
      Label {
        text: qsTr("Interface")
        font.bold: true
      }
      ComboBox {
        id:    interfaceItems
        model: interfaceListModel
        onCurrentIndexChanged: packetListener.interface = currentText
        width: 250
      }

      Label {
        text: qsTr("Filter profile")
        font.bold: true
      }
      ComboBox {
        model: ListModel {
          ListElement { text: "1" }
          ListElement { text: "2" }
          ListElement { text: "3" }
        }
      }

      Button {
        text:      "Start"
        onClicked: packetListener.askedToStart();
        visible:   packetListener.listening != true
      }

      Button {
        text:      "Pause"
        onClicked: packetListener.askedToPause()
        visible:   packetListener.listening
      }
    }

    TableView {
      id: packetLog
      model: packetLogModel
      Layout.fillWidth:  true
      Layout.fillHeight: true

      TableViewColumn {
        role: "alert"
        title: "!"
        width: 25
      }

      TableViewColumn {
        role: "number"
        title: "No."
        width: 50
      }
      TableViewColumn {
        role: "time"
        title: "Time"
        width: 100
      }
      TableViewColumn {
        role: "source"
        title: "Source"
        width: 150
      }
      TableViewColumn {
        role: "destination"
        title: "Destination"
        width: 150
      }
      TableViewColumn {
        role: "protocol"
        title: "Protocol"
        width: 65
      }
      TableViewColumn {
        role: "length"
        title: "Length"
        width: 85
      }
      TableViewColumn {
        role: "information"
        title: "Information"
        width: 500
      }
    }

  }
}
