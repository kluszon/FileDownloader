import QtQuick 2.12
import QtQuick.Controls 2.12
import DownloadState 1.0
import Qt.labs.platform 1.0

Rectangle{
    id: rctButtons
    anchors.left: parent.left
    anchors.right: parent.right
    height: 50
    color: "#666161"
    Row{
        anchors.centerIn: parent
        spacing: 5
        Button{
            width: 100
            height: 30
            text: qsTr("Start")
            enabled: {
                if(fileDownloader.destinationPath.length > 0 && fileDownloader.downloadUrl.length > 0)
                {
                    if(fileDownloader.state === DownloadState.DOWNLOAD_NOT_STARTED ||
                       fileDownloader.state === DownloadState.DOWNLOAD_ABORTED ||
                       fileDownloader.state === DownloadState.DOWNLOAD_FINISHED)
                    {
                        true
                    }else{
                        false
                    }
                }else{
                    false
                }
            }
            onReleased: fileDownloader.download()
        }
        Button{
            width: 100
            height: 30
            text: (fileDownloader.state === DownloadState.DOWNLOAD_IN_PROGRESS) ? qsTr("Pause") : qsTr("Resume")
            enabled: (fileDownloader.state === DownloadState.DOWNLOAD_IN_PROGRESS || fileDownloader.state === DownloadState.DOWNLOAD_PAUSED) ? true : false
            onReleased: (fileDownloader.state === DownloadState.DOWNLOAD_IN_PROGRESS) ? fileDownloader.pause() : fileDownloader.resume()
        }
        Button{
            width: 100
            height: 30
            text: qsTr("Abort")
            enabled: (fileDownloader.state === DownloadState.DOWNLOAD_IN_PROGRESS ||
                      fileDownloader.state === DownloadState.DOWNLOAD_PAUSED) ? true : false
            onReleased: fileDownloader.abort()
        }
        Button{
            width: 100
            height: 30
            text: qsTr("Close")
            onReleased: close()
        }
        CheckBox{
            width: 150
            height: 30
            text: qsTr("Test Data")
            checked: false
            onCheckStateChanged: {
                if(checked){
                    fileDownloader.setDownloadUrl("http://ipv4.download.thinkbroadband.com/1GB.zip")
                    fileDownloader.setDestinationPath(StandardPaths.standardLocations(StandardPaths.HomeLocation)[0].replace("file:///","/"))
                }else{
                    fileDownloader.setDownloadUrl("")
                    fileDownloader.setDestinationPath("/tmp")
                }
            }
        }
    }
}
