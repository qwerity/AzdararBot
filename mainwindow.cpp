#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "AzdararParser.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->logs->setOpenExternalLinks(true);

  initSignalSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::initSignalSlots() {
  connect(ui->actionQuit, &QAction::triggered, [](){ qApp->quit(); });

  connect(ui->grabNews, &QPushButton::clicked, [this](){
    connect(&azdararParser, &AzdararParser::rssParsed, [this](const QVector<AzdararItem> &azdararRSS){
      const auto& keywords = ui->keywords->text().toLower();
      QString html;

      for(const auto &item : azdararRSS) {
        if (item.isEmpty()) { continue;}

        if (!keywords.isEmpty() && !item.title.toLower().contains(keywords)
                                && !item.description.toLower().contains(keywords)) {
          continue;
        }

        html.append(item.htmlBlock());
      }
      ui->logs->setHtml(html);
    });
    azdararParser.rss();
  });
}
