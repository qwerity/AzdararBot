#include "AzdararParser.h"
#include <QDebug>

AzdararParser::AzdararParser()
{
    connect(&downloadManager, &DownloadManager::data, this, &AzdararParser::rssParse);
}

void AzdararParser::rssParse(const QByteArray &data)
{
    QVector<AzdararItem> items;
    xmlReader = new QXmlStreamReader(data);

    while (xmlReader->readNextStartElement()) {
        qDebug() << xmlReader->name();
        if (xmlReader->name() == "channel") {
            while (xmlReader->readNextStartElement()) {
                if (xmlReader->name() == "item") {
                    AzdararItem item;
                    while (xmlReader->readNextStartElement()) {
                        const auto name = xmlReader->name();
                        const auto text = xmlReader->readElementText();

                        if ("title" == name) { item.title = text; }
                        else if ("description" == name) { item.description = text; }
                        else if ("link" == name) { item.link = text; }
                        else if ("pubDate" == name) { item.pubDate = text; }
                        else {
                            qDebug() << "Unexpected item: " << name << " " << text;
                        }
                    }
                    items.append(item);
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

    Q_EMIT rssParsed(items);
}

void AzdararParser::rss()
{
    downloadManager.doDownload(QUrl("https://www.azdarar.am/rss/"));
}

void AzdararParser::searchParse(const QByteArray &data)
{

}

void AzdararParser::search()
{
    downloadManager.doDownload(QUrl("https://www.azdarar.am/search/"));
}

