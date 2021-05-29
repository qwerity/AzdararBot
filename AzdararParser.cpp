#include "AzdararParser.h"
#include <QDebug>

AzdararParser::AzdararParser() {
  connect(&downloadManager, &DownloadManager::data, this, &AzdararParser::rssParse);
}

void AzdararParser::rssParse(const QByteArray &data) {
  xmlReader = new QXmlStreamReader(data);

  QString html;
  while(xmlReader->readNextStartElement()) {
    qDebug() << xmlReader->name();
    if (xmlReader->name() == "channel") {
      while(xmlReader->readNextStartElement()) {
        if (xmlReader->name() == "item") {
          html.append("<p>");
          while(xmlReader->readNextStartElement()) {
            if ("link" != xmlReader->name()) {
              html.append(QString("<strong>%1:</strong> %2<br>").arg(xmlReader->name(), xmlReader->readElementText()));
            } else {
              html.append(QString("<a href='%2'><strong>%1:</strong></a><br>").arg(xmlReader->name(), xmlReader->readElementText()));
            }
          }
          html.append("</p>");
        }
        else {
          qDebug() << xmlReader->name() << xmlReader->readElementText();
        }
      }
    }
  }

  if (xmlReader->hasError()) {
    qDebug() << "Error: Failed to parse xml " << qPrintable(xmlReader->errorString());
  }

  Q_EMIT rssParsed(html);
}

void AzdararParser::rss(const QString &filter) {
  mFilter = filter;
  downloadManager.doDownload(QUrl("https://www.azdarar.am/rss/"));
}

void AzdararParser::searchParse(const QByteArray &data) {

}

void AzdararParser::search(const QString &filter) {
  mFilter = filter;
  downloadManager.doDownload(QUrl("https://www.azdarar.am/search/"));
}

