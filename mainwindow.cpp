#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "AzdararParser.h"

#include "Poco/JSON/Parser.h"
#include "Poco/FileStream.h"

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->logs->setOpenExternalLinks(true);

    parseConfigFile();
    initSignalSlots();

    tgManager = std::make_unique<TelegramBotManager>(configJson->get("tg_token").toString());
    tgManager->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::parseConfigFile(const QString &configFilePath)
{
    try
    {
        Poco::FileInputStream fis(configFilePath.toStdString());
        Poco::JSON::Parser parser;
        auto results = parser.parse(fis);
        configJson = results.extract<Poco::JSON::Object::Ptr>();
    }
    catch(Poco::JSON::JSONException &jsone)
    {
        std::cout << jsone.message();
    }
}

void MainWindow::initSignalSlots()
{
    connect(ui->actionQuit, &QAction::triggered, []()
    { qApp->quit(); });

    connect(ui->grabNews, &QPushButton::clicked, [this]()
    {
        connect(&azdararParser, &AzdararParser::rssParsed, [this](const QVector<AzdararItem> &azdararRSS)
        {
            const auto &keywords = ui->keywords->text().toLower();
            QString html;

            for (const auto &item : azdararRSS) {
                if (item.isEmpty()) { continue; }

                if (!keywords.isEmpty() && !item.title.toLower().contains(keywords)
                    && !item.description.toLower().contains(keywords)) {
                    continue;
                }

                html.append(item.htmlBlock());
            }
            if (!html.isEmpty()) {
                ui->logs->setHtml(html);
            }
            else {
                ui->logs->setHtml("no results with these keywords");
            }
        });
        azdararParser.rss();
    });
}
