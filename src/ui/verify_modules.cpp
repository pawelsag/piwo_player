#include "packettransferview.h"
#include "ui_packettransferview.h"
#include "misc.h"
#include <future>
#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <QMetaObject>

Q_DECLARE_METATYPE(QTextCursor)

PacketTransferView::PacketTransferView(QWidget *parent, Controller* controller):
    QWidget(parent), ui(new Ui::Feedback)
{
    this->controller = controller;
    auto[rows, cols] = this->controller->getSceneSize();
    this->rowCount = rows;
    this->colCount = cols;
    this->ui->setupUi(this);
    this->ui->startButton->setEnabled(false);
    this->rowCurrent = 0;
    this->colCurrent = 0;
    this->logs = std::make_unique<QPlainTextEdit>();

    this->ui->scrollArea_2->setWidget(logs.get());
    logs->setReadOnly(true);

    this->isVerifyCalled = false;
    this->isActiveState = false;
    this->ethernetErrorOccured = false;
    this->deviceSuspended = false;
    this->currentModule = 0;
    connect(&buttonsGroup,  SIGNAL(buttonClicked(QAbstractButton*) ), this, SLOT(onButtonClicked(QAbstractButton*)));

    this->resizeDevice(colCount, rowCount);
    this->ui->scrollAreaWidgetContents->setLayout(this->ui->gridLayout);

    qRegisterMetaType<size_t>("QTextCursor");
    clearButtonsColor();
}

void
PacketTransferView::resizeDevice(size_t x_size, size_t y_size)
{
  rowCount = y_size;
  colCount = x_size;

  auto width = this->geometry().size().height();
  auto buttonSize = QSize(width/5, width/5);
  size_t maxSize = colCount * rowCount;
  buttonsVec.clear();

  for (size_t i = 0; i < maxSize; i++) {
      size_t idx = calculateProperIndex(i);
      buttonsVec.emplace_back(std::make_unique<QPushButton>(QString::number(idx)));
      buttonsVec[i]->setFixedSize(buttonSize);
      buttonsVec[i]->setEnabled(false);
      this->ui->gridLayout->addWidget(buttonsVec[i].get(), i/colCount, i % colCount);
      this->buttonsGroup.addButton(buttonsVec[i].get(), i);
      buttonsVec[i]->setAutoFillBackground(true);
  }

  this->moduleStatuses.resize(colCount*rowCount);
}


void PacketTransferView::clearButtonsColor()
{
    size_t idx;
    size_t maxSize = colCount * rowCount;
    for (size_t i = 0; i < maxSize; i++) {
        idx = calculateProperIndex(i);
        QMetaObject::invokeMethod(this->buttonsVec[idx].get(),
                                  "setStyleSheet",
                                  Qt::QueuedConnection,
                                  Q_ARG(const QString&,"background-color: rgb(225, 225, 225); color: rgb(0, 0, 0)"));
    }
}

size_t PacketTransferView::calculateProperIndex(int i)
{
    return ((rowCount - i/colCount) - 1) * colCount + i%colCount;
}

void PacketTransferView::onButtonClicked(QAbstractButton* button)
{
    if(isActiveState)
      return;

    auto buttonId = this->buttonsGroup.id(button);
    if (this->ui->pingRadioButton->isChecked()) {
        logs->insertPlainText("Sending Ping to " + button->text() + "...\n");
        auto rowIdx =  buttonId / this->colCount;
        auto colIdx = buttonId % this->colCount;
        this->sendSinglePacket( rowIdx, colIdx );
    }
    else if (this->ui->statusRequestRadioButton->isChecked()) {
        logs->insertPlainText("Sending Status Request to " + button->text() + "...\n");
    }
    logs->verticalScrollBar()->setValue(logs->verticalScrollBar()->maximum());
}

PacketTransferView::~PacketTransferView()
{
  deviceSuspended = false;
  delete ui;
}

void
PacketTransferView::clearPacketType()
{
  this->ui->buttonGroup_2->setExclusive(false);
  if (this->ui->pingRadioButton->isChecked()) {
    this->ui->pingRadioButton->setAutoExclusive(false);
    this->ui->pingRadioButton->setChecked(false);
    this->ui->pingRadioButton->setAutoExclusive(true);
  } else if (this->ui->statusRequestRadioButton->isChecked()) {
    this->ui->statusRequestRadioButton->setAutoExclusive(false);
    this->ui->statusRequestRadioButton->setChecked(false);
    this->ui->statusRequestRadioButton->setAutoExclusive(true);
  }
  this->ui->buttonGroup_2->setExclusive(true);
  this->packet_type = ETHERNET_PACKET::NOPACKET;
}

void
PacketTransferView::on_multiRadioButton_pressed()
{

  clearButtonsColor();
  size_t maxSize = colCount * rowCount;

  for (size_t i = 0; i < maxSize; i++) {
    buttonsVec[i]->setEnabled(false);
  }
  this->ui->startButton->setEnabled(true);
}

void
PacketTransferView::on_singleRadioButton_pressed()
{

  clearButtonsColor();
  size_t maxSize = colCount * rowCount;

  for (size_t i = 0; i < maxSize; i++) {
    buttonsVec[i]->setEnabled(true);
  }
  this->ui->startButton->setEnabled(false);
}

void
PacketTransferView::on_startButton_clicked()
{
    if (this->ui->startButton->text() == "START")
    {
        clearButtonsColor();
        currentModule = 0;
        this->ui->startButton->setText("STOP");
        this->isActiveState = true;
        if(this->packet_type == TCP_PACKET_TYPE::PING){
          std::thread t(&PacketTransferView::pingMulti, this);
          t.detach();
        }else{
            qDebug() << "Startring RSSI";
            std::thread t(&PacketTransferView::rssi, this);
            t.detach();
        }
    }else{
        this->ui->startButton->setText("START");
        this->isActiveState = false;
    }
}

bool
compareSeqNumbers(uint8_t rec, uint8_t sent)
{
  return rec == sent;
}

bool PacketTransferView::compareCrc(uint8_t *data, const uint8_t packetType)
{
    switch(packetType) {
        case TCP_PACKET_TYPE::PING:
            return data[Payload::PacketACK::CRC] 
                == crc8(data, data[Payload::PacketACK::PACKET_LENGTH]);
            break;
        case TCP_PACKET_TYPE::STATUS_REQUEST:
            return data[Payload::PacketStatusReplay::CRC] 
                == crc8(data, data[Payload::PacketStatusReplay::PACKET_LENGTH]);
        default:
            return false;
    }
}

void PacketTransferView::paintModule(PingAckStatus status, int16_t moduleNum)
{
    switch(status) {
    case PingAckStatus::StatusOk:
        QMetaObject::invokeMethod(this->buttonsVec[moduleNum].get(), "setStyleSheet", Qt::QueuedConnection,
        Q_ARG(const QString&,"background-color: rgb(0, 255, 0); color: rgb(255, 255, 255)"));
        break;

    case PingAckStatus::StatusLagged:
        QMetaObject::invokeMethod(this->buttonsVec[moduleNum].get(), "setStyleSheet", Qt::QueuedConnection,
        Q_ARG(const QString&,"background-color: rgb(255, 255, 0); color: rgb(255, 255, 255)"));
        break;

    case PingAckStatus::StatusLost:
        QMetaObject::invokeMethod(this->buttonsVec[moduleNum].get(),"setStyleSheet", Qt::QueuedConnection,
        Q_ARG(const QString&,"background-color: rgb(255, 0, 0); color: rgb(255, 255, 255)"));
        break;
      default:
        break;
    }
}

void
PacketTransferView::verify(uint8_t* data)
{
  isVerifyCalled = true;

  if (compareCrc(data, packet_type) == true)
  {
      uint8_t recSeqNumber = data[2];

      module::uid_t recUIDs;
      if(data[PACKET_COMMON_FIELD_ID::PACKET_TYPE] == ETHERNET_PACKET::ACK)
        recUIDs.set_raw(data + Payload::PacketACK::UID);
      else if(data[PACKET_COMMON_FIELD_ID::PACKET_TYPE] == ETHERNET_PACKET::STATUS_REPLY) 
      recUIDs.set_raw(data + Payload::PacketACK::UID);
      else
        return;

      const auto [cell, valid] = controller->findCellByUID(recUIDs);
      const auto &expectedModule = controller->getLookupTable().at(rowCurrent, colCurrent);
      if(!valid)
        return;

      if (compareSeqNumbers(recSeqNumber, sentSeqNumber) &&
              recUIDs ==expectedModule.uid) {

          moduleStatuses[cell.buttonIndex].okCounter++;

          if (moduleStatuses[cell.buttonIndex].okCounter == 5) {
              paintModule(PingAckStatus::StatusOk, cell.buttonIndex);
              moduleStatuses[cell.buttonIndex].okCounter = 0;
          }
          return;
      }
      else if (!compareSeqNumbers(recSeqNumber, sentSeqNumber) ||
               recUIDs != expectedModule.uid) {
          moduleStatuses[cell.buttonIndex].laggedCounter++;
          moduleStatuses[cell.buttonIndex].lostCounter--;

          if (moduleStatuses[cell.buttonIndex].laggedCounter == 5) {
              paintModule(PingAckStatus::StatusLagged, cell.buttonIndex);
              moduleStatuses[cell.buttonIndex].laggedCounter = 0;
          }
          return;
      }
      paintModule(PingAckStatus::StatusLost, cell.buttonIndex);
      moduleStatuses[cell.buttonIndex].lostCounter++;
      return;
  }
  return;
}

void PacketTransferView::pingMulti()
{
  this->isActiveState = true;
    currentModule = 0;
    while (true) {
        for(rowCurrent = rowCount-1; rowCurrent < rowCount; rowCurrent-- ) {
            for(colCurrent = 0; colCurrent < colCount; colCurrent++) {
                logs->insertPlainText("Sending Ping to " + QString::number(currentModule) + "...\n");
                for (size_t i = 0; i < 5; i++) {
                    while(deviceSuspended) std::this_thread::sleep_for(PacketTransferView::THREAD_RELIEF_TIME);

                    sentSeqNumber = controller->getPacketCounter();
                    controller->appendPayload(rowCurrent, colCurrent);
                    controller->sendData();

                    std::this_thread::sleep_for(delay);
                    if (isVerifyCalled == false) {
                        moduleStatuses[currentModule].lostCounter++;
                    }
                    if (!this->isActiveState) {
                        this->ui->startButton->setText("START");
                        return;
                    }
                    if(ethernetErrorOccured){
                        this->isActiveState = false;
                        this->ui->startButton->setText("START");
                        return;
                    }
                }
                if (currentModule == rowCount*colCount-1) {
                    this->isActiveState = false;
                    this->ui->startButton->setText("START");
                    return;
                }
                currentModule++;
            }
        }
    }
   this->isActiveState = false;
}

void PacketTransferView::sendSinglePacket(size_t row, size_t col)
{
    size_t idx, counter = 0;
    this->isActiveState = true;

    rowCurrent = row;
    colCurrent = col;

    for (size_t i = 0; i < 5; i++) {
        while(deviceSuspended) std::this_thread::sleep_for(PacketTransferView::THREAD_RELIEF_TIME);
        controller->appendPayload(rowCurrent, colCurrent);
        controller->sendData();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (isVerifyCalled == false) {
            idx = calculateProperIndex(counter);
            moduleStatuses[idx].lostCounter++;
        }
        if(ethernetErrorOccured){
            this->isActiveState = false;
            return;
        }
    }
    this->isActiveState = false;
}

void PacketTransferView::finishCommunication()
{
    ethernetErrorOccured = true;
    auto duration = std::chrono::duration(std::chrono::milliseconds(200));

    while(this->isActiveState)
        std::this_thread::sleep_for(duration);

    this->ethernetErrorOccured = false;
}


void PacketTransferView::rssi()
{
    this->isActiveState = true;
    while(true){
      for(size_t i = 0; i < rowCount; i++ ) {
          for(size_t j = 0; j < colCount; j++) {
              while(deviceSuspended) std::this_thread::sleep_for(PacketTransferView::THREAD_RELIEF_TIME);
              controller->appendPayload(i, j);
              if(this->isActiveState == false)
                  return;
              if(ethernetErrorOccured){
                  this->isActiveState = false;
                  return;
              }
          }
          std::this_thread::sleep_for(delay);
      }
      controller->sendData();
  }
}


void PacketTransferView::on_delay_valueChanged(int value)
{
  delay = std::chrono::milliseconds{value};
  this->ui->delay_value->setText(QString::number(value));
}

void PacketTransferView::on_statusRequestRadioButton_released()
{
  this->packet_type = ETHERNET_PACKET::STATUS_REQUEST;
}

void PacketTransferView::on_pingRadioButton_released()
{
  this->packet_type = ETHERNET_PACKET::PING;
}

void PacketTransferView::on_rssiButton_released()
{
  this->packet_type = ETHERNET_PACKET::RSSI;
}
