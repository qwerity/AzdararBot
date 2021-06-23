#include "telegram_bot_manager.h"

#include <Poco/StringTokenizer.h>

TelegramBotManager::TelegramBotManager(const std::string &token)
    : bot(TgBot::Bot(token))
{
    initMyCommands();

    dbManager.moveToThread(this);

    bot.getEvents().onCommand("keywords", [this](const TgBot::Message::Ptr &message)
    {
        auto userKeywords = dbManager.userKeywords(message->chat->id);
        auto args = Poco::StringTokenizer(message->text, " ");
        if (args.count() < 3) {
            const static std::string usage = "You can add or delete keyword from your request using follow command:\n\t/keywords add keyword1 keyword2\n\t/keywords del keyword1 keyword2";
            bot.getApi().sendMessage(message->chat->id, usage);
        }
        else {
            for (auto i = 2; i < args.count(); ++i) {
                auto const keyword = QString::fromStdString(args[i]).trimmed();
                if ("add" == args[1]) {
                    if (! userKeywords.contains(keyword)) {
                        userKeywords.append(keyword);
                    }
                }
                else if ("del" == args[1]) {
                    userKeywords.removeAll(keyword);
                }
            }

            dbManager.updateUserKeywords(message->chat->id, userKeywords);
        }
        bot.getApi().sendMessage(message->chat->id, userKeywords.isEmpty() ? "keywords not set" : userKeywords.join(" ").toStdString());
    });
    bot.getEvents().onCommand("reports", [this](const TgBot::Message::Ptr &message)
    {
        const auto userKeywords = dbManager.userKeywords(message->chat->id);
        auto items = azdararParser.rssParsedSync();

        for (auto &item : items) {
            if (item.isEmpty()) { continue; }

            if (!userKeywords.isEmpty() && ! item.containsAtLeastOneKeywords(userKeywords)) { continue; }

            try {
                bot.getApi().sendMessage(message->chat->id, item.tgBlock());
            }
            catch(std::exception &e) {
                qInfo() << e.what();
            }
        }
    });
    bot.getEvents().onCommand("subscribe", [this](const TgBot::Message::Ptr &message)
    {
        dbManager.subscribeUser(message->chat->id);

        const auto userKeywords = dbManager.userKeywords(message->chat->id).join(" ").toStdString();
        bot.getApi().sendMessage(message->chat->id, "Your are subscribed to get reports filtered with keywords: " + userKeywords);
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

void TelegramBotManager::initMyCommands()
{
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "keywords";
    cmdArray->description = "Set keywords to filter request (Separated by space, eg. աճուրդ դատարան)";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "reports";
    cmdArray->description = "Get reports filtered by keywords";
    commands.push_back(cmdArray);

    cmdArray = TgBot::BotCommand::Ptr(new TgBot::BotCommand);
    cmdArray->command = "subscribe";
    cmdArray->description = "You will get every day reports";
    commands.push_back(cmdArray);

    bot.getApi().setMyCommands(commands);
}

void TelegramBotManager::checkSubscriptions()
{
    auto ttime = time(nullptr);
    auto local_time = localtime(&ttime);

    if (local_time->tm_hour == 19) {
        const auto subscriptions = dbManager.subscribedUsers();
        if (! subscriptions.isEmpty()) {
            auto rssItems = azdararParser.rssParsedSync();

            QMapIterator<qint64, QStringList> it(subscriptions);
            while (it.hasNext()) {
                it.next();
                const auto chatID = it.key();
                const auto keywords = it.value();
                qDebug() << chatID << keywords;

                for (auto &item : rssItems) {
                    if (item.isEmpty()) { continue; }

                    if (!keywords.isEmpty() && ! item.containsAtLeastOneKeywords(keywords)) { continue; }

                    try {
                        bot.getApi().sendMessage(chatID, item.tgBlock());
                    }
                    catch(std::exception &e) {
                        qInfo() << e.what();
                    }
                }
            }
        }
    }
}

void TelegramBotManager::run()
{
    try {
        auto lastSubscriptionCheckDateTime = QDateTime::currentDateTime();

        dbManager.open("app.db");
        printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
        bot.getApi().deleteWebhook();

        TgBot::TgLongPoll longPoll(bot);
        while (!stopped) {
            printf("Long poll started\n");
            longPoll.start();

            auto currentDateTime = QDateTime::currentDateTime();
            if (lastSubscriptionCheckDateTime.date() != currentDateTime.date()) {
                checkSubscriptions();
                lastSubscriptionCheckDateTime = currentDateTime;
            }
        }
        dbManager.close();
    }
    catch (std::exception &e) {
        printf("error: %s\n", e.what());
    }
}