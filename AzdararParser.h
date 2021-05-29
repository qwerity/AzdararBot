#pragma once

#include "download_manager.h"
#include <QXmlStreamReader>

class AzdararParser : public QObject
{
  Q_OBJECT

public:
  explicit AzdararParser();
  ~AzdararParser() override = default;

  void search(const QString &filter);
  void rss(const QString &filter);

signals:
  void rssParsed(const QString& data);
  void searchParsed(const QString& data);

private:
  void rssParse(const QByteArray& data);
  void searchParse(const QByteArray& data);

private:
  QXmlStreamReader *xmlReader = Q_NULLPTR;
  DownloadManager downloadManager;

  QString mFilter;
};
