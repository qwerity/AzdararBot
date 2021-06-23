#pragma once

#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlQueryModel>

class DBManager: public QObject
{
    Q_OBJECT

public:
    DBManager() = default;
    ~DBManager() override;

    [[nodiscard]] bool isOpen() const { return m_db.isOpen(); }

    // DB management
    bool open(const QString &dbPath);
    void close();

    bool updateUserSubscription(qint64 chat_id, const QStringList &keywords);
    QStringList userSubscriptionKeywords(qint64 chat_id);

    typedef struct AzdararBotSubscriptions {
        QString chat_id = "chat_id";
        QString keywords = "keywords";
    } AzdararBotSubscriptions;

    struct tablesAndColumns {
        AzdararBotSubscriptions azdararBotSubscriptions;
    };

private:
    bool prepareTables();

private:
    QSqlDatabase m_db;
    const tablesAndColumns m_allTables;

    const QString AzdararBotSubscriptionsTableName = "AzdararBotSubscriptions";
};
