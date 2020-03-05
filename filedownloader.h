/*!
 * \class FileDownloader
 * \brief Class to manage downloading file form web using HTTP protocol.
 *
 * It's use to control downloading files from HTTP and HTTPS pages.
 *
 * \author Michał Kluska
 * \version 1.0
 * \date 2019/09/30
 */

#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QDir>

/*!
 * \brief The DownloadEnum class
 */

class DownloadEnum : public QObject{
    Q_OBJECT
public:

    /*!
     * \brief The DownloadState enum
     */
    enum DownloadState{
        DOWNLOAD_NOT_STARTED = 0,
        DOWNLOAD_IN_PROGRESS,
        DOWNLOAD_PAUSED,
        DOWNLOAD_ABORTED,
        DOWNLOAD_FINISHED
    };
    Q_ENUMS(DownloadState)
};

class FileDownloader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)                                               ///< Download progress
    Q_PROPERTY(QString downloadUrl READ downloadUrl WRITE setDownloadUrl NOTIFY downloadUrlChanged)                                 ///< Download url
    Q_PROPERTY(bool serverAcceptRange READ serverAcceptRange WRITE setServerAcceptRange NOTIFY serverAcceptRangeChanged)            ///< Server accept range downlaoding
    Q_PROPERTY(qint64 downloadTotalSize READ downloadTotalSize WRITE setDownloadTotalSize NOTIFY downloadTotalSizeChanged)          ///< Download total size
    Q_PROPERTY(qint64 downloadCurrentSize READ downloadCurrentSize WRITE setDownloadCurrentSize NOTIFY downloadCurrentSizeChanged)  ///< Download current size
    Q_PROPERTY(qint64 downloadPauseSize READ downloadPauseSize WRITE setDownloadPauseSize NOTIFY downloadPauseSizeChanged)          ///< Download pause size
    Q_PROPERTY(DownloadEnum::DownloadState state READ state WRITE setState NOTIFY stateChanged)        ///< Download state

public:
    explicit FileDownloader(QObject* parent = nullptr);
    ~FileDownloader();

    Q_INVOKABLE void download(QUrl url, QString newDestinationPath);    ///< Start download
    Q_INVOKABLE bool pause();                       ///< Pause download
    Q_INVOKABLE void resume();                      ///< Resume download
    Q_INVOKABLE void abort();                       ///< Abort download

    float progress() const;                 ///< Get progress
    QString downloadUrl() const;            ///< Get download url
    bool serverAcceptRange() const;         ///< Get server accept range
    QString destinationPath() const;        ///< Destination path
    void setDestinationPath(const QString &destinationPath);    ///< Set destination path
    qint64 downloadTotalSize() const;       ///< Download total size
    qint64 downloadCurrentSize() const;     ///< Download current size
    qint64 downloadPauseSize() const;       ///< Download pasue size
    DownloadEnum::DownloadState state() const;      ///< Download state

public slots:
    void setProgress(float progress);                               ///< Set progress
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal); ///< Download progress from QNetworkRequest
    void setDownloadUrl(QString downloadUrl);                       ///< Set download url
    void setServerAcceptRange(bool serverAcceptRange);              ///< Set server accpet range
    void setDownloadTotalSize(qint64 downloadTotalSize);            ///< Set download total size
    void setDownloadCurrentSize(qint64 downloadCurrentSize);        ///< Set download current size
    void setDownloadPauseSize(qint64 downloadPauseSize);            ///< Set download pasue size
    void setState(DownloadEnum::DownloadState state);               ///< Set download state

signals:
    void progressChanged(float progress);                           ///< Progress changed
    void downloadUrlChanged(QString downloadUrl);                   ///< Download url changed
    void downloadCompleted();                                       ///< Download completed
    void serverAcceptRangeChanged(bool serverAcceptRange);          ///< Server accept range changed
    void downloadingInProgressChanged(bool downloadingInProgress);  ///< Downloading in progress changed
    void downloadTotalSizeChanged(qint64 downloadTotalSize);        ///< Download total size changed
    void downloadCurrentSizeChanged(qint64 downloadCurrentSize);    ///< Download current size changed
    void downloadPauseSizeChanged(qint64 downloadPauseSize);        ///< Download pasue size changed
    void stateChanged(DownloadEnum::DownloadState state);           ///< Download state changed

private slots:
    void finished();            ///< download finished
    void finishedFirst();       ///< download first part finished
    void download();            ///< continue download after resume
    void error(QNetworkReply::NetworkError code);   ///< download error

private:
    QNetworkAccessManager *m_networkAccessManager;      ///< network access manager
    QNetworkRequest m_networkRequest;                   ///< network request
    QNetworkReply *m_networkReply;                      ///< network replay
    QFile *m_file;                                      ///< local file

    qint64 m_downloadTotalSize;                         ///< download total size in Byte
    bool m_serverAcceptRange;                           ///< server accept download file from certain byte
    qint64 m_downloadCurrentSize;                       ///< download size in Byte
    qint64 m_downloadPauseSize;                         ///< download size at pause
    float m_progress;                                   ///< download progress
    QString m_downloadUrl;                              ///< download url
    QString m_fileName;                                 ///< file name
    QString m_destinationPath;                          ///< destination path
    DownloadEnum::DownloadState m_state;                ///< download state
};

#endif // FILEDOWNLOADER_H
