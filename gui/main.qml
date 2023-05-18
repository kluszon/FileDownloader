import QtQuick 2.12
import QtQuick.Window 2.12

import "qrc:/gui/window_content"

Window {
    visible: true
    width: 640
    height: 370
    title: qsTr("File downloader") + " " + Qt.application.version
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    Rectangle{
        anchors.fill: parent
        color: "#8c8686"
        Item{
            anchors.fill: parent
            anchors.margins: 10
            ActionButtonSection{
                id: buttonSection
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
            }
            WindowBody{
                id: widnowBody
                anchors.top: buttonSection.bottom
                anchors.topMargin: 20
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
            }
        }
    }
}
