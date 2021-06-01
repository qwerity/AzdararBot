#include "telegram_bot_manager.h"

TelegramBotManager::TelegramBotManager(const std::string &token)
    : bot(TgBot::Bot(token))
{
    initMyCommands();

    bot.getEvents().onCommand("keywords", [this](const TgBot::Message::Ptr &message)
    {
        const auto keywords = botCommandKeywords.join(" ");
        bot.getApi().sendMessage(message->chat->id, keywords.isEmpty() ? "keywords not set" : keywords.toStdString());
    });
    bot.getEvents().onCommand("subscribe", [this](const TgBot::Message::Ptr &message)
    {
        auto items = azdararParser.rssParsedSync();

        for (auto &item : items) {
            if (item.isEmpty()) { continue; }

            if (! item.containsAtLeastOneKeywords(botCommandKeywords)) { continue; }

            try {
                bot.getApi().sendMessage(message->chat->id, item.tgBlock());
            }
            catch(std::exception &e) {
                qInfo() << e.what();
            }
        }
    });
    bot.getEvents().onAnyMessage([&](const TgBot::Message::Ptr &message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/")) {
            return;
        }
        try {
            botCommandKeywords.append(QString::fromStdString(message->text).toLower());
            bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        }
        catch(std::exception &e) {
            qInfo() << e.what();
        }
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
