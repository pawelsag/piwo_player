#ifndef TXDEPLOY_H
#define TXDEPLOY_H

#include <memory>

#include <QWidget>
#include <QtWidgets>

namespace Ui {
class deploy_rx;
}
class Controller;

class deploy_rx : public QWidget
{
  Q_OBJECT

public:
  deploy_rx(QWidget* parent);

  ~deploy_rx();

private slots:

  void gridSelectionChanged();
  void on_deleteTx_released();
  void showEvent(QShowEvent* event);
  void keyReleaseEvent(QKeyEvent* event);

  void on_cleareAll_released();

  void on_sendConfig_released();

  void on_visibleRadioCheckBox_stateChanged(int arg1);

  void on_radio_id_currentIndexChanged(int index);

  void on_channelComboBox_currentIndexChanged(int index);

  bool getAckState() { return false; }

  void on_sendRadioButton_clicked();

  void on_radioState_stateChanged(int state);

private:
  Ui::deploy_rx* ui;
};

#endif // TXDEPLOY_H
