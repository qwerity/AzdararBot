#pragma once

#include "download_manager.h"
#include <QXmlStreamReader>

#include "Poco/Net/HTTPSStreamFactory.h"

static std::string rssURL("https://www.azdarar.am/rss");
static std::string searchURL("https://www.azdarar.am/search");

struct AzdararItem
{
    QString title;
    QString description;
    QString link;
    QString pubDate;

    QString tags;

    bool isEmpty() const
    { return title.isEmpty() && description.isEmpty(); }

    bool containsAtLeastOneKeywords(const QStringList &keywords)
    {
        bool contains = false;
        for (const auto& keyword : keywords) {
            if (!title.contains(keyword, Qt::CaseInsensitive) && !description.contains(keyword, Qt::CaseInsensitive)) {
                continue;
            }
            else {
                contains = true;
                tags.append("#" + keyword + " ");
            }
        }
        return contains;
    }

    QString htmlBlock() const
    { return QString("<p>%1: <a href='%2'><strong>%3</strong></a></p>").arg(pubDate, link, title); }

    std::string tgBlock() const
    { return QString("%1\n%2\n%3\n%4").arg(pubDate, title, link, tags).toStdString(); }
};

class AzdararParser: public QObject
{
    Q_OBJECT

public:
    explicit AzdararParser();
    ~AzdararParser() override = default;

    void search();
    void rss();
    QVector<AzdararItem> rssParsedSync();

signals:
    void rssParsed(const QVector<AzdararItem> &azdararRSS);
    void searchParsed(const QString &data);

private:
    QVector<AzdararItem> rssParse(const QByteArray &data);
    void searchParse(const QByteArray &data);

private:
    QXmlStreamReader *xmlReader = Q_NULLPTR;
    DownloadManager downloadManager;

    QMap<std::string, QVector<AzdararItem>> azdararResults;

    std::unique_ptr<Poco::Net::HTTPSStreamFactory> httpsStreamFactory = nullptr;
};
