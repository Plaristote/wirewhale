// Toolbar.qml
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

RowLayout {
  spacing: 6

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
