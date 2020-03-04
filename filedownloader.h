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

class FileDownloader : public QObject
{
    Q_OBJECT

    Q_PROPERTY(float progress READ progress WRITE setProgress NOTIFY progressChanged)                                       ///< Download progress
    Q_PROPERTY(bool downloadingInProgress READ downloadingInProgress WRITE setDownloadingInProgress NOTIFY downloadingInProgressChanged)    ///< Downloading in progress
    Q_PROPERTY(QString downloadUrl READ downloadUrl WRITE setDownloadUrl NOTIFY downloadUrlChanged)                         ///< Download url
    Q_PROPERTY(bool serverAcceptRange READ serverAcceptRange WRITE setServerAcceptRange NOTIFY serverAcceptRangeChanged)    ///< Server accept range downlaoding
    Q_PROPERTY(qint64 downloadTotalSize READ downloadTotalSize WRITE setDownloadTotalSize NOTIFY downloadTotalSizeChanged)
    Q_PROPERTY(qint64 downloadCurrentSize READ downloadCurrentSize WRITE setDownloadCurrentSize NOTIFY downloadCurrentSizeChanged)
    Q_PROPERTY(qint64 downloadPauseSize READ downloadPauseSize WRITE setDownloadPauseSize NOTIFY downloadPauseSizeChanged)

public:
    explicit FileDownloader(QObject* parent = nullptr);
    ~FileDownloader();

    Q_INVOKABLE void download(QUrl url, QString newDestinationPath);            ///< Start download
    Q_INVOKABLE bool pause();                       ///< Pause download
    Q_INVOKABLE void resume();                      ///< Resume download
    Q_INVOKABLE void abort();                       ///< Abort download

    float progress() const;                 ///< Get progress
    QString downloadUrl() const;            ///< Get download url
    bool serverAcceptRange() const;         ///< Get server accept range
    bool downloadingInProgress() const;     ///< Downloading in progress
    QString destinationPath() const;
    void setDestinationPath(const QString &destinationPath);
    qint64 downloadTotalSize() const;
    qint64 downloadCurrentSize() const;
    qint64 downloadPauseSize() const;

public slots:
    void setProgress(float progress);                               ///< Set progress
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal); ///< Download progress from QNetworkRequest
    void setDownloadUrl(QString downloadUrl);                       ///< Set download url
    void setServerAcceptRange(bool serverAcceptRange);              ///< Set server accpet range
    void setDownloadingInProgress(bool downloadingInProgress);      ///< Set downloading in progress
    void setDownloadTotalSize(qint64 downloadTotalSize);
    void setDownloadCurrentSize(qint64 downloadCurrentSize);
    void setDownloadPauseSize(qint64 downloadPauseSize);

signals:
    void progressChanged(float progress);                   ///< Progress changed
    void downloadUrlChanged(QString downloadUrl);           ///< Download url changed
    void downloadCompleted();                               ///< Download completed
    void serverAcceptRangeChanged(bool serverAcceptRange);  ///< Server accept range changed
    void downloadingInProgressChanged(bool downloadingInProgress);  ///< Downloading in progress
    void downloadTotalSizeChanged(qint64 downloadTotalSize);
    void downloadCurrentSizeChanged(qint64 downloadCurrentSize);
    void downloadPauseSizeChanged(qint64 downloadPauseSize);

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
    qint64 m_downloadPauseSize;                            ///< download size at pause
    float m_progress;                                   ///< download progress
    QString m_downloadUrl;                              ///< download url
    QString m_fileName;                                 ///< file name
    bool m_downloadingInProgress;                       ///< downloading in progress
    QString m_destinationPath;                          ///< destination path
};

#endif // FILEDOWNLOADER_H
