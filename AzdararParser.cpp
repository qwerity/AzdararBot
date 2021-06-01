#include "AzdararParser.h"
#include <QDebug>

#include "Poco/URIStreamOpener.h"
#include "Poco/StreamCopier.h"
#include "Poco/URI.h"


AzdararParser::AzdararParser()
{
    httpsStreamFactory = std::unique_ptr<Poco::Net::HTTPSStreamFactory>(new Poco::Net::HTTPSStreamFactory);

    connect(&downloadManager, &DownloadManager::data, this, &AzdararParser::rssParse);
}

QVector<AzdararItem> AzdararParser::rssParse(const QByteArray &data)
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
    return items;
}

void AzdararParser::rss()
{
    downloadManager.doDownload(QUrl(QString::fromStdString(rssURL)));
}

void AzdararParser::searchParse(const QByteArray &data)
{

}

void AzdararParser::search()
{
    downloadManager.doDownload(QUrl(QString::fromStdString(searchURL)));
}

QVector<AzdararItem> AzdararParser::rssParsedSync()
{
    // Create the URI from the URL to the file.
    static Poco::URI uri(rssURL);
    std::string buffer{};

    // Open the stream and copy the data to the buffer.
    auto pStr = std::unique_ptr<std::istream>(httpsStreamFactory->open(uri));
    Poco::StreamCopier::copyToString(*pStr, buffer);

    return rssParse(QByteArray::fromStdString(buffer));
}
