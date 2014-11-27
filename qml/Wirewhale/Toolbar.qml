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
    enabled:  packetListener.listening != true

    function onCurrentTextChanged(currentText) {
      packetListener.interface = currentText
    }
  }

  LabelledComboBox {
    label:    qsTr("Filter profile")
    model:    wirewhale.profileList
    minWidth: 200
    enabled:  packetListener.listening != true

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
    Layout.alignment: Qt.AlignBottom
  }

  Button {
    text:      qsTr("Start")
    onClicked: packetListener.askedToStart()
    visible:   packetListener.listening != true
    Layout.alignment: Qt.AlignBottom
  }

  Button {
    text:      qsTr("Pause")
    onClicked: packetListener.askedToPause()
    visible:   packetListener.listening == true
    Layout.alignment: Qt.AlignBottom
  }

  ColumnLayout {
    id: leLayout
    opacity: 1
    visible: opacity > 0
    FadeEffect { fadeTarget: leLayout ; visible2: window.width > 625 }

    Label {
      text: "Maximum entries"
    }

    TextField {
      id: maxPackets
      x: 1203
      y: 115
      text: packetLogModel.maxPackets
      inputMethodHints: Qt.ImhDigitsOnly
      placeholderText: qsTr("Max Entries")
      onTextChanged: packetLogModel.maxPackets = maxPackets.text
    }
  }

  Button {
    id:            clearButton
    anchors.right: parent.right
    text:          qsTr("Clear")
    onClicked:     packetLogModel.clear()
    Layout.alignment: Qt.AlignBottom
  }
}
