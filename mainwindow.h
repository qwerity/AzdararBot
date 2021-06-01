#pragma once

#include <QMainWindow>

#include "AzdararParser.h"
#include "telegram_bot_manager.h"

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
    void initSignalSlots();

private:
    Ui::MainWindow *ui = Q_NULLPTR;
    AzdararParser azdararParser;
    TelegramBotManager tgManager;
};
