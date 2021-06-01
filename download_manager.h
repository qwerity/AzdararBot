#pragma once

#include <QtCore>
#include <QtNetwork>

QT_BEGIN_NAMESPACE
class QSslError;
QT_END_NAMESPACE

class DownloadManager: public QObject
{
    Q_OBJECT

public:
    DownloadManager();
    void doDownload(const QUrl &url);
    static bool isHttpRedirect(QNetworkReply *reply);

signals:
    void data(const QByteArray &data);

public slots:
    void downloadFinished(QNetworkReply *reply);
    static void sslErrors(const QList<QSslError> &errors);

private:
    QNetworkAccessManager manager;
    QVector<QNetworkReply *> currentDownloads;
};
