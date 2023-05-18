import QtQuick 2.12
import QtQuick.Controls 2.12
import Qt.labs.platform 1.0
import DownloadState 1.0

Item {    
    Label{
        id: lblFileToCopyPath
        anchors.top: parent.top
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
        validator: RegExpValidator{
            regExp: /^https?:\/\/(www\.)?[-a-zA-Z0-9@:%._\+~#=]{1,256}\.[a-zA-Z0-9()]{1,6}\b([-a-zA-Z0-9()@:%_\+.~#?&//=]*)$/
        }
        text: fileDownloader.downloadUrl
        placeholderText: "http://your-download-url.com"
        onTextChanged: {
            if(acceptableInput){
                fileDownloader.setDownloadUrl(text)
            }else{
                fileDownloader.setDownloadUrl("")
            }
        }
        background: Rectangle{
            anchors.fill: parent
            border.width: 2
            border.color:{
                if(tfFilePath.length === 0){
                    "white"
                }else{
                    if(tfFilePath.acceptableInput){
                        "green"
                    }else{
                        "red"
                    }
                }
            }
        }
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
        text: fileDownloader.destinationPath
        placeholderText: "/your-destination-path"
        onPressed: {
            folderDialog.open()
        }
        onTextChanged: fileDownloader.setDestinationPath(text)
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
        id: pbDownloading
        anchors.top: lblProgress.bottom
        anchors.topMargin: 10
        width: parent.width
        height: 30
        value: fileDownloader.progress/100
        padding: 2

        background: Rectangle {
            implicitWidth: pbDownloading.width
            implicitHeight: pbDownloading.height
            color: "#e6e6e6"
            radius: 3
        }
        contentItem: Item {
            implicitWidth: pbDownloading.width
            implicitHeight: pbDownloading.height

            Rectangle {
                width: pbDownloading.visualPosition * parent.width
                height: parent.height
                radius: 2
                color: "#17a81a"
            }
        }
        Label{
            anchors.centerIn: parent
            text: (fileDownloader.progress).toFixed(2) + " %"
            z: 10
        }
    }
    Label{
        anchors.top: pbDownloading.bottom
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        text: (fileDownloader.state === DownloadState.DOWNLOAD_PAUSED ? (fileDownloader.downloadPauseSize / 1000000).toFixed(2) : (fileDownloader.downloadCurrentSize / 1000000).toFixed(2))
              + "MB / " +
              (fileDownloader.downloadTotalSize / 1000000).toFixed(2) + " MB"
    }
    FolderDialog {
        id: folderDialog
        folder: ""
        onFolderChanged: {
            tfDestinationDir.text = folderDialog.folder.toString().replace("file:///","/")
        }
    }
}
