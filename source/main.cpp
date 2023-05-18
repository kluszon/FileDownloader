#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtNetwork>

#include "source/file_downloader/filedownloader.h"

int main(int argc, char* argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    FileDownloader *fileDownloader = new FileDownloader();

    qmlRegisterType<DownloadEnum>("DownloadState", 1, 0, "DownloadState");

    engine.rootContext()->setContextProperty("fileDownloader", fileDownloader);

    engine.load(QUrl(QStringLiteral("qrc:/gui/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
