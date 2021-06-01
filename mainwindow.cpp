#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "AzdararParser.h"

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    ///TODO(ksh): move token to json config file
    tgManager("1628964068:AAGPtBbk9D5Bq7nnucPYENYkGlrIuEHy-cU")
{
    ui->setupUi(this);

    ui->logs->setOpenExternalLinks(true);

    initSignalSlots();
    tgManager.start();
}

MainWindow::~MainWindow()
{
    delete ui;
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
