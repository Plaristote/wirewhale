import QtQuick 2.0
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.0

RowLayout {
    property string label: 'label'
    property var    model

    spacing: parent.spacing

    function onCurrentTextChanged() {
    }

    Label {
      text: parent.label
      font.bold: true
    }

    ComboBox {
      model: parent.model
      onCurrentIndexChanged: parent.onCurrentTextChanged(currentText)
    }
}
