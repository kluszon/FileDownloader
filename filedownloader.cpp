#include "filedownloader.h"
#include <QtMath>


/*!
 * \brief Default constructor
 *
 * Initialize default values for variables.
 *
 */

FileDownloader::FileDownloader(QObject *parent)
    : QObject(parent),
      m_networkAccessManager(nullptr),
      m_networkReply(nullptr),
      m_file(nullptr),
      m_downloadTotalSize(0),
      m_serverAcceptRange(false),
      m_downloadCurrentSize(0),
      m_downloadPauseSize(0),
      m_progress(0),
      m_downloadUrl(""),
      m_fileName(""),
      m_destinationPath("/tmp"),
      m_state(DownloadEnum::DOWNLOAD_NOT_STARTED)
{
}

/*!
 * \brief Destructor
 *
 * Stop downloading.
 *
 */

FileDownloader::~FileDownloader(){
    if(m_networkReply != nullptr){
        pause();
    }
}

/*!
 * \brief Download
 *
 * Start download form url and create connections.
 *
 * \param url - QUrl download file url
 *
 */

void FileDownloader::download(QUrl url, QString newDestinationPath)
{
    setState(DownloadEnum::DOWNLOAD_IN_PROGRESS);
    setDestinationPath(newDestinationPath);

    m_downloadUrl = url.toString();
    {
        QFileInfo fileInfo(url.toString());
        m_fileName = fileInfo.fileName();
    }
    setDownloadCurrentSize(0);
    setDownloadPauseSize(0);

    m_networkAccessManager = new QNetworkAccessManager();
    m_networkRequest = QNetworkRequest(url);

    m_networkReply = m_networkAccessManager->head(m_networkRequest);

    connect(m_networkReply, SIGNAL(finished()), this, SLOT(finishedFirst()));
    connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

/*!
 * \brief Pause
 *
 * Pause download and close connections. Flush any buffer to file.
 *
 */

bool FileDownloader::pause()
{
    setState(DownloadEnum::DOWNLOAD_PAUSED);

    if( m_networkReply == nullptr){
        return true;
    }

    disconnect(m_networkReply, SIGNAL(finished()), this, SLOT(finished()));
    disconnect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    disconnect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    m_networkReply->abort();
    m_file->flush();
    QNetworkReply *temporaryRelay = m_networkReply;
    temporaryRelay->deleteLater();
    m_networkReply = nullptr;
    setDownloadPauseSize(m_downloadCurrentSize);
    setDownloadCurrentSize(0);

    return true;
}

/*!
 * \brief Resume
 *
 * Resume download.
 *
 */

void FileDownloader::resume()
{
    setState(DownloadEnum::DOWNLOAD_IN_PROGRESS);
    setDownloadCurrentSize(m_downloadPauseSize);
    download();
}

/*!
 * \brief Abort
 *
 * Abort download, delete QNetworkReplay object, close local file and remove it.
 *
 */


void FileDownloader::abort()
{
    setProgress(0.0);

    if(!m_networkReply) return;

    if(pause()){
        setServerAcceptRange(false);
        m_file->close();
        m_file->remove();
    }

    setState(DownloadEnum::DOWNLOAD_ABORTED);
}

/*!
 * \brief Download
 *
 * Continue download file after resume.
 *
 */

void FileDownloader::download()
{
    if (m_serverAcceptRange)
    {
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(m_downloadPauseSize) + "-";
        if (m_downloadTotalSize > 0)
        {
            rangeHeaderValue += QByteArray::number(m_downloadTotalSize);
        }
        m_networkRequest.setRawHeader("Range", rangeHeaderValue);
    }

    m_networkReply = m_networkAccessManager->get(m_networkRequest);

    connect(m_networkReply, SIGNAL(finished()), this, SLOT(finished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(downloadProgress(qint64, qint64)));
    connect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));
}

/*!
 * \brief Finished first part
 *
 * Finished first part of file.
 *
 */

void FileDownloader::finishedFirst()
{
    setServerAcceptRange(false);

    if (m_networkReply->hasRawHeader("Accept-Ranges"))
    {
        QString qstrAcceptRanges = m_networkReply->rawHeader("Accept-Ranges");
        setServerAcceptRange(qstrAcceptRanges.compare("bytes", Qt::CaseInsensitive) == 0);
    }

    setDownloadTotalSize(m_networkReply->header(QNetworkRequest::ContentLengthHeader).toInt());

    m_networkRequest.setRawHeader("Connection", "Keep-Alive");
    m_networkRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);

    QString newFilePath = m_destinationPath + QDir::separator() + m_fileName + ".part";

    QFile::remove(newFilePath);
    m_file = new QFile(newFilePath);

    if (!m_serverAcceptRange)
    {
        m_file->remove();
    }
    m_file->open(QIODevice::ReadWrite | QIODevice::Append);

    setDownloadPauseSize(m_file->size());
    download();
}

/*!
 * \brief Finished
 *
 * Finished download file.
 *
 */

void FileDownloader::finished()
{
    m_file->close();

    QFile::remove(m_fileName);

    QFile::rename(m_destinationPath + QDir::separator() + m_fileName + QString(".part"),
                  m_destinationPath + QDir::separator() + m_fileName);


    m_file = NULL;
    QNetworkReply *temporaryRelay = m_networkReply;
    temporaryRelay->deleteLater();
    m_networkReply = nullptr;
    setState(DownloadEnum::DOWNLOAD_FINISHED);

    emit downloadCompleted();
}

/*!
 * \brief Download progress
 *
 * Get download progress and calculate it on percents.
 *
 * \param bytesReceived - qint64 bytes received
 * \param bytesTotal - qint64 bytes total
 *
 */

void FileDownloader::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    setDownloadCurrentSize(m_downloadPauseSize + bytesReceived);

    m_file->write(m_networkReply->readAll());

    float percentProgress = static_cast<float>((static_cast<float>(m_downloadPauseSize + bytesReceived) * 100.0) / static_cast<float>(m_downloadPauseSize + bytesTotal));

    setProgress(percentProgress);
}

/*!
 * \brief Progress
 *
 * Get progress.
 *
 */

float FileDownloader::progress() const
{
    return m_progress;
}

/*!
 * \brief Download url
 *
 * Get download url.
 *
 */

QString FileDownloader::downloadUrl() const
{
    return m_downloadUrl;
}

/*!
 * \brief Server accept range
 *
 * Get server accept range.
 *
 */

bool FileDownloader::serverAcceptRange() const
{
    return m_serverAcceptRange;
}

/*!
 * \brief Set progress
 *
 * Set progress.
 *
 */

void FileDownloader::setProgress(float progress)
{
    if (m_progress == progress)
        return;

    m_progress = progress;
    emit progressChanged(m_progress);
}

/*!
 * \brief Set download url
 *
 * Set download url.
 *
 */

void FileDownloader::setDownloadUrl(QString downloadUrl)
{
    if (m_downloadUrl == downloadUrl)
        return;

    m_downloadUrl = downloadUrl;
    emit downloadUrlChanged(m_downloadUrl);
}

/*!
 * \brief Set Server Accept Range
 *
 * Set server accept range
 *
 */

void FileDownloader::setServerAcceptRange(bool serverAcceptRange)
{
    if (m_serverAcceptRange == serverAcceptRange)
        return;

    m_serverAcceptRange = serverAcceptRange;
    emit serverAcceptRangeChanged(m_serverAcceptRange);
}

/*!
 * \brief FileDownloader::setDownloadTotalSize
 * \param downloadTotalSize
 */

void FileDownloader::setDownloadTotalSize(qint64 downloadTotalSize)
{
    if (m_downloadTotalSize == downloadTotalSize)
        return;

    m_downloadTotalSize = downloadTotalSize;
    emit downloadTotalSizeChanged(m_downloadTotalSize);
}

/*!
 * \brief FileDownloader::setDownloadCurrentSize
 * \param downloadCurrentSize
 */

void FileDownloader::setDownloadCurrentSize(qint64 downloadCurrentSize)
{
    if (m_downloadCurrentSize == downloadCurrentSize)
        return;

    m_downloadCurrentSize = downloadCurrentSize;
    emit downloadCurrentSizeChanged(m_downloadCurrentSize);
}

/*!
 * \brief FileDownloader::setDownloadPauseSize
 * \param downloadPauseSize
 */

void FileDownloader::setDownloadPauseSize(qint64 downloadPauseSize)
{
    if (m_downloadPauseSize == downloadPauseSize)
        return;

    m_downloadPauseSize = downloadPauseSize;
    emit downloadPauseSizeChanged(m_downloadPauseSize);
}

/*!
 * \brief FileDownloader::setState
 * \param state
 */

void FileDownloader::setState(DownloadEnum::DownloadState state)
{
    if (m_state == state)
        return;

    m_state = state;
    emit stateChanged(m_state);
}

/*!
 * \brief Error
 *
 * Debug error.
 *
 */

void FileDownloader::error(QNetworkReply::NetworkError code)
{
    qDebug() << "Error:"<<code;
}

/*!
 * \brief FileDownloader::destinationPath
 * \return
 */

QString FileDownloader::destinationPath() const
{
    return m_destinationPath;
}

/*!
 * \brief FileDownloader::setDestinationPath
 * \param destinationPath
 */

void FileDownloader::setDestinationPath(const QString &destinationPath)
{
    m_destinationPath = destinationPath;
}

/*!
 * \brief FileDownloader::downloadTotalSize
 * \return
 */

qint64 FileDownloader::downloadTotalSize() const
{
    return m_downloadTotalSize;
}

/*!
 * \brief FileDownloader::downloadCurrentSize
 * \return
 */

qint64 FileDownloader::downloadCurrentSize() const
{
    return m_downloadCurrentSize;
}

/*!
 * \brief FileDownloader::downloadPauseSize
 * \return
 */

qint64 FileDownloader::downloadPauseSize() const
{
    return m_downloadPauseSize;
}

/*!
 * \brief FileDownloader::state
 * \return
 */

DownloadEnum::DownloadState FileDownloader::state() const
{
    return m_state;
}
