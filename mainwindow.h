#pragma once

#include <QMainWindow>
#include "AzdararParser.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  Ui::MainWindow *ui = Q_NULLPTR;
  AzdararParser azdararParser;
};
