#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  connect(ui->actionQuit, &QAction::triggered, [this](){ qApp->quit(); });

  connect(ui->grabNews, &QPushButton::clicked, [this](){
    downloadManager.doDownload(QUrl(ui->newsRSSUrl->text()));
  });

  connect(&downloadManager, &DownloadManager::data, [this](const QByteArray& data){
    if(data.contains(ui->keywords->text().toLocal8Bit())){
      qDebug() << "asda";
    }
    QString d(data);
//    qDebug() << QString::fromLatin1(data);
  });
}

MainWindow::~MainWindow()
{
    delete ui;
}
