#include <QCoreApplication>

#include "Poco/JSON/Parser.h"
#include "Poco/FileStream.h"

#include "telegram_bot_manager.h"

Poco::JSON::Object::Ptr
parse_config_file(const QString &configFilePath)
{
    try
    {
        Poco::FileInputStream fis(configFilePath.toStdString());
        Poco::JSON::Parser parser;
        auto results = parser.parse(fis);
        return results.extract<Poco::JSON::Object::Ptr>();
    }
    catch(Poco::JSON::JSONException &jsone)
    {
        std::cout << jsone.message();
        return nullptr;
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    auto config_json = parse_config_file("config.json");

    std::unique_ptr<TelegramBotManager> tg_bot_manager = std::make_unique<TelegramBotManager>(config_json->get("tg_token").toString());
    tg_bot_manager->start();

    return QCoreApplication::exec();
}
