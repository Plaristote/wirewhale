import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0

ApplicationWindow {
  title: qsTr("Wirewhale")
  width: 800
  height: 600

  menuBar: MenuBar {
    Menu {
      title: qsTr("File")
      MenuItem {
        text: qsTr("Exit")
        onTriggered: Qt.quit();
      }
    }
  }

  ColumnLayout {
    anchors.fill: parent;

    RowLayout {
      id: toolbar
      anchors.top: parent.top;
      Label {
        text: qsTr("Interface")
        font.bold: true
      }
      ComboBox {
        model: ListModel {
          id: cbItems
          ListElement { text: "eth0";  }
          ListElement { text: "eth1";  }
          ListElement { text: "usb0";  }
          ListElement { text: "wlan0"; }
        }
      }

      Label {
        text: qsTr("Filter profile")
        font.bold: true
      }
      ComboBox {

      }
    }

    TableView {
      id: packetLog
      objectName: "packetLog"
      anchors.top: toolbar.bottom
      anchors.bottom: parent.bottom
      anchors.left: parent.left
      anchors.right: parent.right

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
