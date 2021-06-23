#include "telegram_bot_manager.h"

#include <Poco/StringTokenizer.h>

TelegramBotManager::TelegramBotManager(const std::string &token)
    : bot(TgBot::Bot(token))
{
    initMyCommands();

    dbManager.moveToThread(this);

    bot.getEvents().onCommand("keywords", [this](const TgBot::Message::Ptr &message)
    {
        auto userSubscriptionKeywords = dbManager.userSubscriptionKeywords(message->chat->id);
        auto args = Poco::StringTokenizer(message->text, " ");
        if (args.count() < 3) {
            const static std::string usage = "you can add or delete keyword from your request using follow command:\n\t/keywords add keyword1 keyword2\n\t/keywords del keyword1 keyword2";
            bot.getApi().sendMessage(message->chat->id, usage);
        }
        else {
            for (auto i = 2; i < args.count(); ++i) {
                auto const keyword = QString::fromStdString(args[i]);
                if ("add" == args[1]) {
                    if (! userSubscriptionKeywords.contains(keyword)) {
                        userSubscriptionKeywords.append(keyword);
                    }
                }
                else if ("del" == args[1]) {
                    userSubscriptionKeywords.removeAll(keyword);
                }
            }

            dbManager.updateUserSubscription(message->chat->id, userSubscriptionKeywords);
        }
        bot.getApi().sendMessage(message->chat->id, userSubscriptionKeywords.isEmpty() ? "keywords not set" : userSubscriptionKeywords.join(" ").toStdString());
    });
    bot.getEvents().onCommand("subscribe", [this](const TgBot::Message::Ptr &message)
    {
        const auto userSubscriptionKeywords = dbManager.userSubscriptionKeywords(message->chat->id);
        auto items = azdararParser.rssParsedSync();

        for (auto &item : items) {
            if (item.isEmpty()) { continue; }

            if (!userSubscriptionKeywords.isEmpty() && ! item.containsAtLeastOneKeywords(userSubscriptionKeywords)) { continue; }

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
//            botCommandKeywords.append(QString::fromStdString(message->text).toLower());
//            bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
        }
        catch(std::exception &e) {
            qInfo() << e.what();
        }
    });
}

TelegramBotManager::~TelegramBotManager()
{
    stopped = true;
    if (!wait(11000)) {
        terminate();
    }
}

void TelegramBotManager::run()
{
    try {
        dbManager.open("app.db");
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
