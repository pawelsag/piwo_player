#ifndef WINDOWSETTINGS_H
#define WINDOWSETTINGS_H

#include "controller.h"
#include "bassrecord.h"
#include "midi.h"
#include "txdeploy.h"
#include "txplainconfig.h"

#include <QWidget>
#include "midi.h"

namespace Ui {
class WindowSettings;
}

class WindowSettings : public QWidget
{
  Q_OBJECT

public:
  explicit WindowSettings(QWidget* parent, Controller* controller);
  void holdLaunchpadCommunication();
  Q_INVOKABLE void disconnectDevice(int r1);
  int8_t loadDevice(QString address);
  void cleanDevices();
  void removeDevice(int8_t r1);
  midi_info getMidiSettings();
  ~WindowSettings();
public slots:

  void connect_to_launchpad_device();
  void disconnect_from_launchpad_device();
  void searchForAudioInDevices();
  void searchForAudioOutDevices();
  void change_size_of_grids();

private slots:
  void on_AudioInConnect_released();

  void on_AudioInDisconnet_released();

  void on_AudioInName_currentIndexChanged(int index);

  void on_TxConfig_released();

  void on_AuidoOutConnect_released();

  void on_AudioOutDisconnet_released();

  void on_AudioOutName_currentIndexChanged(int index);

  void on_removeDevice_released();

  void on_connectDevice_released();

  void on_disconnectDevice_released();

  void on_MidiConnect_clicked();

  void on_MidiDisconnect_clicked();

  void on_mainConfig_released();

  void on_pushButton_released();

private:
  Ui::WindowSettings* ui;
  Controller* controller;
  std::unique_ptr<deploy_rx> TxIPManager;
  std::unique_ptr<configure_tx> planeConfig;

  bool LaunchpadSusspend;
  void renderDeviceList(const Containers::DeviceArray& deviceList);

};

#endif // WINDOWSETTINGS_H
