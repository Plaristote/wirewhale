import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2

TableView {
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

