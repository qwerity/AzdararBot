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
    QSqlQuery query;
    query.prepare("CREATE TABLE IF NOT EXISTS " + AzdararBotSubscriptionsTableName + "("
       + m_allTables.azdararBotSubscriptions.chat_id  + " INTEGER PRIMARY KEY, "
       + m_allTables.azdararBotSubscriptions.keywords + " TEXT );"
    );

    if (!query.exec()) {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
        return false;
    }

    return true;
}

bool DBManager::updateUserSubscription(qint64 chat_id, const QStringList &keywords)
{
    QSqlQuery query;
    query.prepare("REPLACE INTO " + AzdararBotSubscriptionsTableName
                    + " (chat_id, keywords) values (:chat_id, :keywords)");
    query.bindValue(":chat_id", chat_id);
    query.bindValue(":keywords", keywords.join(" "));

    if (!query.exec()) {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
        return false;
    }

    return true;
}

QStringList DBManager::userSubscriptionKeywords(qint64 chat_id)
{
    QStringList keywords;
    QSqlQuery query;
    query.prepare("SELECT keywords FROM " + AzdararBotSubscriptionsTableName
                       + " WHERE chat_id = :chat_id"
    );
    query.bindValue(":chat_id", chat_id);

    if (query.exec())
    {
        if (query.first()) {
            int idKeywords = query.record().indexOf("keywords");
            auto keywordsStr = query.value(idKeywords).toString();
            keywords = keywordsStr.split(" ");
        }
    }
    else {
        qDebug() << __FUNCTION__ << query.lastQuery();
        qDebug() << __FUNCTION__ << query.lastError();
    }

    return keywords;
}
