#include "db_manager.h"
#include <QDebug>

DBManager::~DBManager()
{
    close();
}

bool DBManager::open(const QString &dbPath)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Error: connection with database fail" << "db path: " << dbPath;
        return false;
    } else { qDebug() << "Database: connection ok"; }

    prepareTables();
    return true;
}

void DBManager::close()
{
    m_db.close();
}

bool DBManager::prepareTables()
{
    const auto queryStr = QString("CREATE TABLE IF NOT EXISTS %1 ("
                                  " %2 INTEGER PRIMARY KEY,"
                                  " %3 TEXT,"
                                  " %4 INTEGER DEFAULT 0);")
        .arg(AzdararBotSubscriptionsTableName,
             m_allTables.azdararBotSubscriptions.chat_id, m_allTables.azdararBotSubscriptions.keywords,
             m_allTables.azdararBotSubscriptions.subscribed);

    QSqlQuery query;
    query.prepare(queryStr);

    if (!query.exec()) {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
        return false;
    }

    return true;
}

bool DBManager::updateUserKeywords(qint64 chat_id, const QStringList &keywords)
{
    QSqlQuery query;
    const auto updateQueryStr = QString("UPDATE %1 SET %2=? WHERE %3=?").arg(AzdararBotSubscriptionsTableName,
                                                                             m_allTables.azdararBotSubscriptions.keywords,
                                                                             m_allTables.azdararBotSubscriptions.chat_id);
    query.prepare(updateQueryStr);
    query.addBindValue(keywords.join(" "));
    query.addBindValue(chat_id);
    if (!query.exec() || 0 == query.numRowsAffected()) {
        const auto insertQueryStr = QString("INSERT INTO %1 (%2, %3) values (?, ?)")
            .arg(AzdararBotSubscriptionsTableName, m_allTables.azdararBotSubscriptions.chat_id, m_allTables.azdararBotSubscriptions.keywords);

        query.prepare(insertQueryStr);
        query.addBindValue(chat_id);
        query.addBindValue(keywords.join(" "));

        if (!query.exec()) {
            qDebug() << __FUNCTION__ << query.lastQuery();
            qDebug() << __FUNCTION__ << query.lastError();
            return false;
        }
    }

    return true;
}

QStringList DBManager::userKeywords(qint64 chat_id)
{
    const auto queryStr = QString("SELECT %1 FROM %2 WHERE chat_id = ?")
        .arg(m_allTables.azdararBotSubscriptions.keywords, AzdararBotSubscriptionsTableName);

    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(chat_id);

    QStringList keywords;
    if (query.exec())
    {
        if (query.first()) {
            int idKeywords = query.record().indexOf(m_allTables.azdararBotSubscriptions.keywords);
            auto keywordsStr = query.value(idKeywords).toString();
            if (! keywordsStr.isEmpty()) {
                keywords = keywordsStr.split(" ");
            }
        }
    }
    else {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
    }

    return keywords;
}

QMap<qint64, QStringList> DBManager::subscribedUsers()
{
    const auto queryStr = QString("SELECT %1, %2 FROM %3 WHERE %4 = ?")
        .arg(m_allTables.azdararBotSubscriptions.chat_id, m_allTables.azdararBotSubscriptions.keywords,
             AzdararBotSubscriptionsTableName,
             m_allTables.azdararBotSubscriptions.subscribed);

    QSqlQuery query;
    query.prepare(queryStr);
    query.addBindValue(true);

    if (! query.exec()) {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
        return {};
    }

    QMap<qint64, QStringList> subscriptions;
    while (query.next())
    {
        const auto idChatID = query.record().indexOf(m_allTables.azdararBotSubscriptions.chat_id);
        const auto idKeywords = query.record().indexOf(m_allTables.azdararBotSubscriptions.keywords);

        const auto chatID = query.value(idChatID).toInt();
        const auto keywords = query.value(idKeywords).toString().split(" ");
        subscriptions.insert(chatID, keywords);
    }

    return subscriptions;
}

bool DBManager::subscribeUser(qint64 chat_id)
{
    QSqlQuery query;
    const auto updateQueryStr = QString("UPDATE %1 SET %2=? WHERE %3=?").arg(AzdararBotSubscriptionsTableName,
                                                                  m_allTables.azdararBotSubscriptions.subscribed,
                                                                  m_allTables.azdararBotSubscriptions.chat_id);
    query.prepare(updateQueryStr);
    query.addBindValue(true);
    query.addBindValue(chat_id);
    if (!query.exec() || 0 == query.numRowsAffected()) {
        const auto insertQueryStr = QString("INSERT INTO %1 (%2, %3) values (?, ?)")
            .arg(AzdararBotSubscriptionsTableName, m_allTables.azdararBotSubscriptions.chat_id, m_allTables.azdararBotSubscriptions.subscribed);

        query.prepare(insertQueryStr);
        query.addBindValue(chat_id);
        query.addBindValue(true);

        if (!query.exec()) {
            qDebug() << __FUNCTION__ << query.lastQuery();
            qDebug() << __FUNCTION__ << query.lastError();
            return false;
        }
    }

    return true;
}
