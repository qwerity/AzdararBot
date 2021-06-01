#include "telegram_bot_manager.h"

TelegramBotManager::TelegramBotManager(const std::string &token)
    : bot(TgBot::Bot(token))
{
    initMyCommands();

    bot.getEvents().onCommand("keywords", [this](const TgBot::Message::Ptr &message)
    {
        botCommandKeywords.append(QString::fromStdString(message->text));
    });
    bot.getEvents().onCommand("subscribe", [this](const TgBot::Message::Ptr &message)
    {
        messageChatID = message->chat->id;
        azdararParser.rss();
    });
    bot.getEvents().onAnyMessage([&](const TgBot::Message::Ptr &message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }
        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });
}

void TelegramBotManager::initSignalSlots()
{
    connect(&azdararParser, &AzdararParser::rssParsed, [this](const QVector<AzdararItem> &azdararRSS)
    {
        const auto &keywords = botCommandKeywords.constFirst().toLower();
        QString html;

        for (const auto &item : azdararRSS) {
            if (item.isEmpty()) { continue; }

            if (!keywords.isEmpty() && !item.title.toLower().contains(keywords)
                && !item.description.toLower().contains(keywords)) {
                continue;
            }

            html.append(item.htmlBlock());
        }
        bot.getApi().sendMessage(messageChatID, html.toStdString());
    });
}


void TelegramBotManager::run()
{
    try {
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(bot);
        while (!stopped) {
            printf("Long poll started\n");
            longPoll.start();
        }
    }
    catch (std::exception &e) {
        printf("error: %s\n", e.what());
    }
}
TelegramBotManager::~TelegramBotManager()
{
    stopped = true;
    if (!wait(11000)) {
        terminate();
    }
}

void TelegramBotManager::initMyCommands()
{
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "keywords";
    cmdArray->description = "Set keywords to filter request (Separated by space, eg. աճուրդ դատարան)";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "subscribe";
    cmdArray->description = "es el comando dos";
    commands.push_back(cmdArray);

    bot.getApi().setMyCommands(commands);
}
