#pragma once

#include "download_manager.h"
#include <QXmlStreamReader>

struct AzdararItem {
  QString title;
  QString description;
  QString link;
  QString pubDate;

  bool isEmpty() const { return title.isEmpty() && description.isEmpty(); }
  QString htmlBlock() const { return QString("<p>%1: <a href='%2'><strong>%3</strong></a></p>").arg(pubDate, link, title); }
};

class AzdararParser : public QObject
{
  Q_OBJECT

public:
  explicit AzdararParser();
  ~AzdararParser() override = default;

  void search();
  void rss();

signals:
  void rssParsed(const QVector<AzdararItem> &azdararRSS);
  void searchParsed(const QString& data);

private:
  void rssParse(const QByteArray& data);
  void searchParse(const QByteArray& data);

private:
  QXmlStreamReader *xmlReader = Q_NULLPTR;
  DownloadManager downloadManager;
};
