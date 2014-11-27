import QtQuick 2.0

Rectangle {
  id: fadeHandler
  property var  fadeTarget
  property bool visible2: true

  visible: false

  PropertyAnimation { id: fadeOut; target: fadeTarget; property: "opacity"; to: 0; duration: 300 }
  PropertyAnimation { id: fadeIn;  target: fadeTarget; property: "opacity"; to: 1; duration: 300 }

  onVisible2Changed: {
    fadeIn.stop();
    fadeOut.stop();
    if (fadeHandler.visible2 == true)
      fadeIn.start();
    else
      fadeOut.start();
  }
}
