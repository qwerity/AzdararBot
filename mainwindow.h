#pragma once

#include <QMainWindow>

#include "AzdararParser.h"
#include "telegram_bot_manager.h"

#include "Poco/JSON/Object.h"

namespace Ui
{
class MainWindow;
}

class MainWindow: public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    void parseConfigFile(const QString &configFilePath = "config.json");
    void initSignalSlots();

private:
    Ui::MainWindow *ui = Q_NULLPTR;
    AzdararParser azdararParser;

    Poco::JSON::Object::Ptr configJson = nullptr;
    std::unique_ptr<TelegramBotManager> tgManager = nullptr;
};
