import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2
import Qt.labs.platform 1.0

Window {
    visible: true
    width: 640
    height: 370
    title: qsTr("File downloader")
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
            Rectangle{
                id: rctButtons
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                height: 100
                color: "#666161"
                Row{
                    anchors.centerIn: parent
                    spacing: 5
                    Button{
                        width: 100
                        height: 30
                        text: "Start"
                        enabled: !fileDownloader.downloadingInProgress
                        onReleased: fileDownloader.download(tfFilePath.text, tfDestinationDir.text)
                    }
                    Button{
                        width: 100
                        height: 30
                        text: "Pause"
                        enabled: fileDownloader.downloadingInProgress
                        onReleased: fileDownloader.pause()
                    }
                    Button{
                        width: 100
                        height: 30
                        text: "Resume"
                        enabled: fileDownloader.downloadingInProgress
                        onReleased:  fileDownloader.resume()
                    }
                    Button{
                        width: 100
                        height: 30
                        text: "Abort"
                        enabled: fileDownloader.downloadingInProgress
                        onReleased: fileDownloader.abort()
                    }
                }
            }
            Label{
                id: lblFileToCopyPath
                anchors.top: rctButtons.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                text: "File to copy path: "
                color: "#e6dbdb"
            }
            TextField{
                id: tfFilePath
                anchors.top: lblFileToCopyPath.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.right: parent.right
                height: 40
                text: "http://ipv4.download.thinkbroadband.com/1GB.zip"
                onEditingFinished: fileDownloader.setDownloadUrl(text)
            }
            Label{
                id: lblDestinationPath
                anchors.top: tfFilePath.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                text: "Destination dir path: "
                color: "#e6dbdb"
            }
            TextField{
                id: tfDestinationDir
                anchors.top: lblDestinationPath.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                anchors.right: parent.right
                height: 40
                text: "/tmp"
                onPressed: {
                    folderDialog.open()
                }
            }
            Label{
                id: lblServerState
                anchors.top: tfDestinationDir.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                text: "Server accept pause download: " +
                      (fileDownloader.serverAcceptRange ? "<font color=\"green\">" :
                                                          "<font color=\"red\">")
                      + fileDownloader.serverAcceptRange + "</font>"
                color: "#e6dbdb"
                MouseArea{
                    id: areaServerState
                    property bool toolTipVisible: false
                    anchors.fill: parent
                    hoverEnabled: true
                    onEntered:{
                        toolTipVisible = true
                    }
                    onExited: {
                        toolTipVisible = false
                    }
                }
                ToolTip.visible: areaServerState.toolTipVisible
                ToolTip.text: qsTr("Checked after start downloading")
            }
            Label{
                id: lblProgress
                anchors.top: lblServerState.bottom
                anchors.topMargin: 10
                anchors.left: parent.left
                text: "Progress: "
                color: "#e6dbdb"
            }
            ProgressBar{
                id: control
                anchors.top: lblProgress.bottom
                anchors.topMargin: 10
                width: parent.width
                height: 30
                value: fileDownloader.progress/100
                padding: 2

                background: Rectangle {
                    implicitWidth: control.width
                    implicitHeight: control.height
                    color: "#e6e6e6"
                    radius: 3
                }
                contentItem: Item {
                    implicitWidth: control.width
                    implicitHeight: control.height

                    Rectangle {
                        width: control.visualPosition * parent.width
                        height: parent.height
                        radius: 2
                        color: "#17a81a"
                    }
                }
                Label{
                    anchors.centerIn: parent
                    text: Math.round(fileDownloader.progress * 100) / 100 + " %"
                    z: 10
                }
            }
        }
    }
    FolderDialog {
        id: folderDialog
        folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        onFolderChanged: {
            tfDestinationDir.text = folderDialog.folder.toString().replace("file:///","/")
        }
    }
}
