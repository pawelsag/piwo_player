#pragma once
#include <QDialog>

namespace Ui {
class configure_tx;
}

class configure_tx : public QDialog
{
  Q_OBJECT

public:
  configure_tx() {}
  configure_tx(QWidget* parent = nullptr);
  void refresh_config();
  ~configure_tx();

private slots:
  void showEvent(QShowEvent* event);
  void keyReleaseEvent(QKeyEvent* event);
  void on_deviceList_currentIndexChanged(int index);
};
