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
    explicit DBManager() = default;
    ~DBManager() override;

    [[nodiscard]] bool isOpen() const { return m_db.isOpen(); }

    // DB management
    bool open(const QString &dbPath);
    void close();

    bool updateUserKeywords(qint64 chat_id, const QStringList &keywords);
    QStringList userKeywords(qint64 chat_id);
    QMap<qint64, QStringList> subscribedUsers();
    bool subscribeUser(qint64 chat_id);

    typedef struct AzdararBotSubscriptions {
        QString chat_id = "chat_id";
        QString keywords = "keywords";
        QString subscribed = "subscribed";
    } AzdararBotSubscriptions;

    struct tablesAndColumns {
        AzdararBotSubscriptions azdararBotSubscriptions;
    };

private:
    bool prepareTables();

private:
    QSqlDatabase m_db;
    tablesAndColumns m_allTables;

    const QString AzdararBotSubscriptionsTableName = "AzdararBotSubscriptions";
};
