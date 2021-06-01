#pragma once

#include "AzdararParser.h"

#include <tgbot/tgbot.h>

#include <QThread>

class TelegramBotManager: public QThread
{
    Q_OBJECT

public:
    explicit TelegramBotManager(const std::string &token);
    ~TelegramBotManager() override;

signals:

private:
    void initMyCommands();
    void run() override;

private:
    TgBot::Bot bot;
    std::vector<TgBot::BotCommand::Ptr> commands;

    AzdararParser azdararParser;

    bool stopped = false;
    QStringList botCommandKeywords;
};
