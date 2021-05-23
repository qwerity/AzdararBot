#include "download_manager.h"

DownloadManager::DownloadManager()
{
  connect(&manager, &QNetworkAccessManager::finished, this, &DownloadManager::downloadFinished);
}

void DownloadManager::doDownload(const QUrl &url)
{
  QNetworkRequest request(url);
  QNetworkReply *reply = manager.get(request);

#if QT_CONFIG(ssl)
  connect(reply, &QNetworkReply::sslErrors, this, &DownloadManager::sslErrors);
#endif

  currentDownloads.append(reply);
}

bool DownloadManager::isHttpRedirect(QNetworkReply *reply)
{
  int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
  return statusCode == 301 || statusCode == 302 || statusCode == 303
      || statusCode == 305 || statusCode == 307 || statusCode == 308;
}

void DownloadManager::sslErrors(const QList<QSslError> &sslErrors)
{
#if QT_CONFIG(ssl)
  for (const QSslError &error : sslErrors) {
    qDebug() << "SSL error: " << qPrintable(error.errorString());
  }
#else
  Q_UNUSED(sslErrors);
#endif
}

void DownloadManager::downloadFinished(QNetworkReply *reply)
{
  QUrl url = reply->url();
  if (reply->error()) {
    qDebug() << "Download of " << url.toEncoded()
             <<" failed: " << qPrintable(reply->errorString());
  } else {
    if (isHttpRedirect(reply)) {
      qDebug() << "Request was redirected.";
    } else {
      Q_EMIT data(reply->readAll());
    }
  }

  currentDownloads.removeAll(reply);
  reply->deleteLater();
}