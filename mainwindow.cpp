#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "AzdararParser.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionQuit, &QAction::triggered, [](){ qApp->quit(); });

  ui->logs->setOpenExternalLinks(true);
  connect(ui->grabNews, &QPushButton::clicked, [this](){
    connect(&azdararParser, &AzdararParser::rssParsed, [this](const QString& data){
      ui->logs->setHtml(data);
    });
    azdararParser.rss(ui->keywords->text());
  });
}

MainWindow::~MainWindow()
{
    delete ui;
}
