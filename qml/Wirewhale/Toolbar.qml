// Toolbar.qml
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

RowLayout {
  spacing: 6

  LabelledComboBox {
    label: qsTr("Interface")
    model: interfaceListModel

    function onCurrentTextChanged(currentText) {
      packetListener.interface = currentText
    }
  }

  LabelledComboBox {
    label: qsTr("Filter profile")
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
