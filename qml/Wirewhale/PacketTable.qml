import QtQuick 2.0
import QtQuick.Controls 1.0

TableView {

  function infoColumnsWidth()
  {
    var columns     = [ alert_column, number_column, time_column ];
    var total_width = 0;

    for (var i = 0 ; i < columns.length ; ++i)
      total_width += columns[i].width;
    return (total_width);
  }

  function getColumnWidthFromRatio(ratio, max_width)
  {
    var width = (parent.width - infoColumnsWidth()) * ratio;

    if (typeof max_width != "undefined" && width > max_width)
      return max_width;
    return width;
  }

  TableViewColumn {
    id: alert_column
    role: "alert"
    title: "!"
    width: 25
  }

  TableViewColumn {
    id: number_column
    role: "number"
    title: "No."
    width: 50
  }

  TableViewColumn {
    id: time_column
    role: "time"
    title: "Time"
    width: 140
  }

  TableViewColumn {
    role: "source"
    title: "Source"
    width: getColumnWidthFromRatio(0.20, 150)
  }
  TableViewColumn {
    role: "destination"
    title: "Destination"
    width: getColumnWidthFromRatio(0.20, 150)
  }
  TableViewColumn {
    role: "protocol"
    title: "Protocol"
    width: getColumnWidthFromRatio(0.15, 65)
  }
  TableViewColumn {
    role: "length"
    title: "Length"
    width: getColumnWidthFromRatio(0.15, 65)
  }
  TableViewColumn {
    role: "information"
    title: "Information"
    width: getColumnWidthFromRatio(0.30)
  }
}

