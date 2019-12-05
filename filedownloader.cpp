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
      m_networkAccessManager(NULL),
      m_networkReply(NULL),
      m_file(NULL),
      m_downloadTotal(0),
      m_serverAcceptRange(false),
      m_downloadSize(0),
      m_downloadSizeAtPause(0),
      m_progress(0),
      m_downloadUrl(""),
      m_fileName(""),
      m_downloadingInProgress(false)
{
}

/*!
 * \brief Destructor
 *
 * Stop downloading.
 *
 */

FileDownloader::~FileDownloader(){
    if(m_networkReply != NULL){
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

void FileDownloader::download(QUrl url)
{
    setDownloadingInProgress(true);

    m_downloadUrl = url.toString();
    {
        QFileInfo fileInfo(url.toString());
        m_fileName = fileInfo.fileName();
    }
    m_downloadSize = 0;
    m_downloadSizeAtPause = 0;

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
    if( m_networkReply == NULL) {
        return true;
    }
    disconnect(m_networkReply, SIGNAL(finished()), this, SLOT(finished()));
    disconnect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    disconnect(m_networkReply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(error(QNetworkReply::NetworkError)));

    m_networkReply->abort();
    m_file->flush();
    m_networkReply = 0;
    m_downloadSizeAtPause = m_downloadSize;
    m_downloadSize = 0;

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
    setDownloadingInProgress(false);

    if(!m_networkReply) return;

    if(pause()){
        setServerAcceptRange(false);
        m_networkReply->deleteLater();
        m_file->close();
        m_file->remove();
    }
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
        QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(m_downloadSizeAtPause) + "-";
        if (m_downloadTotal > 0)
        {
            rangeHeaderValue += QByteArray::number(m_downloadTotal);
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

    qDebug() << "m_downloadTotal: " << m_downloadTotal;

    m_downloadTotal = m_networkReply->header(QNetworkRequest::ContentLengthHeader).toInt();

    m_networkRequest.setRawHeader("Connection", "Keep-Alive");
    m_networkRequest.setAttribute(QNetworkRequest::HttpPipeliningAllowedAttribute, true);
    m_file = new QFile(m_fileName + ".part");
    if (!m_serverAcceptRange)
    {
        m_file->remove();
    }
    m_file->open(QIODevice::ReadWrite | QIODevice::Append);

    m_downloadSizeAtPause = m_file->size();
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
    m_file->rename(m_fileName + ".part", m_fileName);
    m_file = NULL;
    m_networkReply->deleteLater();
    m_networkReply = 0;
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
    m_downloadSize = m_downloadSizeAtPause + bytesReceived;

    m_file->write(m_networkReply->readAll());

    float percentProgress = static_cast<float>((static_cast<float>(m_downloadSizeAtPause + bytesReceived) * 100.0) / static_cast<float>(m_downloadSizeAtPause + bytesTotal));

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
 * \brief Downloading in progress
 *
 * Downloading in progress
 *
 */

bool FileDownloader::downloadingInProgress() const
{
    return m_downloadingInProgress;
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
 * \brief Set downloading in progress
 *
 * Set downloading in progress
 *
 */

void FileDownloader::setDownloadingInProgress(bool downloadingInProgress)
{
    if (m_downloadingInProgress == downloadingInProgress)
        return;

    m_downloadingInProgress = downloadingInProgress;
    emit downloadingInProgressChanged(m_downloadingInProgress);
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
