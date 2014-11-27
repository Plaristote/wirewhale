// Toolbar.qml
import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

RowLayout {
  spacing:       6
  anchors.left:  parent.left
  anchors.right: parent.right

  LabelledComboBox {
    label:    qsTr("Interface")
    model:    wirewhale.interfaceList
    minWidth: 125

    function onCurrentTextChanged(currentText) {
      packetListener.interface = currentText
    }
  }

  LabelledComboBox {
    label:    qsTr("Filter profile")
    model:    wirewhale.profileList
    minWidth: 200

    function onCurrentTextChanged(currentText) {
      packetListener.filter = currentText
    }
  }

  Button {
    onClicked: {
    text: qsTr("Edit profile")
      console.log("implement edit profile dialog")
    }
    visible: wirewhale.profileList.count > 0
  }

  Button {
    text:      qsTr("New profile")
    onClicked: {
        console.log("implement new profile dialog");
    }
  }

  Button {
    text:      qsTr("Start")
    onClicked: packetListener.askedToStart()
    visible:   packetListener.listening != true
  }

  Button {
    text:      qsTr("Pause")
    onClicked: packetListener.askedToPause()
    visible:   packetListener.listening == true
  }

  Button {
    anchors.right: parent.right
    text:          qsTr("Clear")
    onClicked:     packetLogModel.clear()
  }
}
