#include "windowsettings.h"
#include "ui_windowsettings.h"

WindowSettings::WindowSettings(QWidget* parent, Controller* controller)
  : QWidget(parent)
  , ui(new Ui::WindowSettings)
{
  this->controller = controller;
  this->ui->setupUi(this);
  this->TxIPManager = std::make_unique<deploy_rx>(nullptr, controller);
  this->planeConfig = std::make_unique<configure_tx>(nullptr, controller);
  this->controller->registerWidget(this->TxIPManager.get());
  this->searchForAudioInDevices();
  this->searchForAudioOutDevices();

  this->ui->EthernetBox->setVisible(true);

  emit(on_AudioInName_currentIndexChanged(0));
  this->LaunchpadSusspend = false;

  connect(this->ui->LaunchpadConnect,
          SIGNAL(released()),
          this,
          SLOT(connect_to_launchpad_device()));

  connect(this->ui->LaunchpadDisconnect,
          SIGNAL(released()),
          this,
          SLOT(disconnect_from_launchpad_device()));

  connect(this->ui->AudioInRefresh,
          SIGNAL(released()),
          this,
          SLOT(searchForAudioInDevices()));

  connect(this->ui->RES_change,
          SIGNAL(released()),
          this,
          SLOT(change_size_of_grids()));
  emit(on_pushButton_released());
}
void
WindowSettings::searchForAudioOutDevices()
{

  BASS_DEVICEINFO info;
  this->ui->AudioOutName->clear();
  this->ui->AudioOutName->addItem("No Device");
  for (uint a = 1; BASS_GetDeviceInfo(a, &info); a++) {
    if (info.flags & BASS_DEVICE_ENABLED) { // device is enabled
      this->ui->AudioOutName->addItem(QString::fromLocal8Bit(info.name));
    } else
      BassCommunicates::reportEror("searchForAudioOutDevices");
  }
}

void
WindowSettings::searchForAudioInDevices()
{

  BASS_DEVICEINFO info;
  this->ui->AudioInName->clear();
  for (uint a = 0; BASS_RecordGetDeviceInfo(a, &info); a++) {
    if (info.flags & BASS_DEVICE_ENABLED) { // device is enabled
      this->ui->AudioInName->addItem(QString::fromLocal8Bit(info.name));
    } else
      BassCommunicates::reportEror("searchForAudioInDevices");
  }
}

void
WindowSettings::on_connectDevice_released()
{
  QString address = "";
  auto selectedItems = this->ui->deviceList->selectedItems();

  int8_t r1;

  if (selectedItems.size() == 0) {

      address = this->ui->EthernetIP->text();
      if (this->ui->deviceList->findItems(address, Qt::MatchExactly).size() !=
          0) {
        return;
      }
      r1 = this->controller->setupEthernetCommunication(
        address.toStdString().c_str());

    if (r1 == -1)
      return;
  } else {
    auto r2 = this->ui->deviceList->row(selectedItems[0]);
    auto item = this->ui->deviceList->item(r2);
    item->setSelected(true);

    r1 = this->controller->reconnect(r2);
    if (r1 == -1)
      return;
  }

  const auto& devices = this->controller->getDevices();
  this->renderDeviceList(devices);
}

int8_t
WindowSettings::loadDevice(QString address)
{

  int8_t r1;
  r1 = this->controller->setupEthernetCommunication(
    address.toStdString().c_str());

  if (r1 != -1){
    const auto& devices = this->controller->getDevices();
    this->renderDeviceList(devices);
  }

  return r1;
}

void
WindowSettings::on_disconnectDevice_released()
{
  auto r1 = this->ui->deviceList->currentIndex().row();
  if (r1 == -1)
    return;

  auto itemText = this->ui->deviceList->item(r1)->text();
  bool res;
  res = this->controller
         ->closeCommunication<DEVICE_INFO::ERROR_MODE::ON>(r1);

  if (res == false)
    return;

  const auto& devices = this->controller->getDevices();
  this->renderDeviceList(devices);
}

void
WindowSettings::disconnectDevice(int r1)
{
  if (r1 == -1)
    return;

  auto itemText = this->ui->deviceList->item(r1)->text();
  bool res;
  res = this->controller
         ->closeCommunication<DEVICE_INFO::ERROR_MODE::OFF>(r1);

  if (res == false)
    return;

  const auto& devices = this->controller->getDevices();
  this->renderDeviceList(devices);
}

void
WindowSettings::on_removeDevice_released()
{
  auto r1 = this->ui->deviceList->currentIndex().row();
  this->removeDevice(r1);
}

void
WindowSettings::removeDevice(int8_t r1)
{

  if (r1 == -1)
    return;

  auto res = this->controller->removeDevice(r1);

  if (res == false)
    return;

  const auto& devices = this->controller->getDevices();
  this->renderDeviceList(devices);
}


void
WindowSettings::renderDeviceList(const Containers::DeviceArray& deviceList)
{
  QColor color;
  QListWidgetItem* item;
  auto lastDeviceCount = this->ui->deviceList->count();
  for (uint8_t i = 0; i < lastDeviceCount; i++)
    delete this->ui->deviceList->item(0);

  for (uint8_t i = 0; i < Controller::txCount; i++) {
    if (!deviceList[i]->isConnected())
      color = QColor(250, 80, 80);
    else
      color = QColor(100, 230, 100);
    //                         stupid way od doing copy...
    item = new QListWidgetItem(deviceList[i]->getDeviceName());
    item->setBackground(color);
    this->ui->deviceList->insertItem(this->ui->deviceList->count(), item);
  }
}

midi_info WindowSettings::getMidiSettings()
{
    midi_info info;
    std::get<static_cast<int>(midi_info_idx::TRIGGER_START_BASE)>(info) = this->ui->MidiTriggerStart->value();
    std::get<static_cast<int>(midi_info_idx::TRIGGER_STOP_BASE)>(info) = this->ui->MidiTriggerStop->value();
    std::get<static_cast<int>(midi_info_idx::VELOCITY_START)>(info) = this->ui->Start_V->value();
    std::get<static_cast<int>(midi_info_idx::VELOCITY_STOP)>(info) = this->ui->Stop_V->value();
    return info;
}

void
WindowSettings::change_size_of_grids()
{
  controller->resizeGrids(static_cast<uint32_t>(ui->XaxisBox->value()),
                          static_cast<uint32_t>(ui->YAxisBox->value()));
}

void
WindowSettings::connect_to_launchpad_device()
{
   this->controller->setupLaunchpadCommunication(ui->LaunchpadName->text().toStdString().c_str());
}

void
WindowSettings::disconnect_from_launchpad_device()
{
  this->controller->closeLaunchpadCommunication();
}

void
WindowSettings::holdLaunchpadCommunication()
{
  this->ui->LaunchpadConnect->setText(
    "Communication susspended\n ~~~ \nClick to conntinue");
  this->ui->LaunchpadConnect->setStyleSheet("Color:#ff0000");

  this->LaunchpadSusspend = true;
}

WindowSettings::~WindowSettings()
{
  delete ui;
}

void
WindowSettings::on_AudioInConnect_released()
{
  if (this->controller->audioIn.initRecording())
    controller->setupAudioinCommunication();
}

void
WindowSettings::on_AudioInDisconnet_released()
{
  if (this->controller->audioIn.stopRecording())
    controller->closeAudioinCommunication();
}

void
WindowSettings::on_AudioInName_currentIndexChanged(int index)
{
  if (this->ui->AudioInName->itemText(index).length())
    this->controller->audioIn.setDeviceID(index);
  else
    this->controller->audioIn.setDeviceID(-1);
}

void
WindowSettings::on_TxConfig_released()
{
  auto selectedTxNumber = this->ui->deviceList->currentIndex().row();
  if (selectedTxNumber == -1)
    return;
  this->TxIPManager->setTxName(
       this->ui->deviceList->item(selectedTxNumber)->text());
  this->TxIPManager->setTxId(selectedTxNumber);
  this->TxIPManager->show();
}

void
WindowSettings::on_AuidoOutConnect_released()
{
  this->controller->audioOut.initStream();
  this->controller->setupAudiooutCommunication();
}

void
WindowSettings::on_AudioOutDisconnet_released()
{
  this->controller->audioOut.finishStream();
  this->controller->closeAudiooutCommunication();
}

void
WindowSettings::on_AudioOutName_currentIndexChanged(int index)
{
  this->controller->audioOut.setDevice(index);
}

void
WindowSettings::on_mainConfig_released()
{
  if (this->ui->deviceList->count() == 0)
    return;
  planeConfig->show();
}

void WindowSettings::on_MidiConnect_clicked()
{
    if(this->controller->connectMidi(this->ui->midiDeviceList->currentIndex()) ==0){
        this->ui->MidiTriggerStart->setEnabled(false);
        this->ui->MidiTriggerStop->setEnabled(false);
        this->ui->Start_V->setEnabled(false);
        this->ui->Stop_V->setEnabled(false);
    }
}

void WindowSettings::on_MidiDisconnect_clicked()
{
    if( this->controller->disconnectMidi() == 0){
        this->ui->MidiTriggerStart->setEnabled(true);
        this->ui->MidiTriggerStop->setEnabled(true);
        this->ui->Start_V->setEnabled(true);
        this->ui->Stop_V->setEnabled(true);
    }
}

void WindowSettings::on_pushButton_released()
{
    this->ui->midiDeviceList->clear();

    auto devList = Midi::getDeviceList();
    for(auto && dev : devList){
        this->ui->midiDeviceList->addItem(dev.c_str());
    }
}
