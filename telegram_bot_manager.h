#pragma once

#include "AzdararParser.h"
#include "db_manager.h"

#include <tgbot/tgbot.h>

#include <QThread>

class TelegramBotManager: public QThread
{
    Q_OBJECT

public:
    explicit TelegramBotManager(const std::string &token);
    ~TelegramBotManager() override;

private:
    void initMyCommands();
    void checkSubscriptions();
    void run() override;

private:
    TgBot::Bot bot;
    std::vector<TgBot::BotCommand::Ptr> commands;

    DBManager dbManager;
    AzdararParser azdararParser;

    bool stopped = false;
};
