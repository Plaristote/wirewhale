import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

RowLayout {
    property string label: 'label'
    property var    model
    property real   minWidth: 200

    spacing: parent.spacing

    function onCurrentTextChanged() {
    }

    Label {
      id: labelObject
      text: parent.label
      font.bold: true
    }

    ComboBox {
      model: parent.model
      onCurrentIndexChanged: parent.onCurrentTextChanged(currentText)
      Layout.minimumWidth: parent.minWidth - labelObject.width
    }
}
