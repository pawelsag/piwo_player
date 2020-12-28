#ifndef PACKETTRANSFERVIEW_H
#define PACKETTRANSFERVIEW_H

#include "controller.h"
#include "dragableWidgets.h"
#include "helperTypesAliases.h"
#include "ethernetconnection.h"

#include <QtWidgets>
#include <memory>
#include "misc.h"
#include "iPlayerWidget.h"

namespace Ui {
class Feedback;
}

class Controller;

enum class PingAckStatus
{
    StatusOk,
    StatusLagged,
    StatusLost,
    StatusIncorrect
};

struct ModuleStatus
{
    uint8_t okCounter = 0;
    uint8_t laggedCounter = 0;
    uint8_t lostCounter = 0;
};

class PacketTransferView : public QWidget, public iPlayerWidget
{
  constexpr static std::chrono::milliseconds THREAD_RELIEF_TIME {3};
 std::chrono::milliseconds delay {50};
  Q_OBJECT
public:

    PacketTransferView(QWidget *parent, Controller* controller);

    void manageIncomeData();
    void SetupUI();
    Q_INVOKABLE void onButtonClicked(QAbstractButton*);
    void setModelCell(int row, int col, QString uid);
    void pingMulti();
    void verify(uint8_t* data);
    ~PacketTransferView();
    void clearPacketType();
    void rssi();
    void sendSinglePacket(size_t row, size_t col);

    Q_INVOKABLE void resizeDevice(size_t x_size, size_t y_size);
    void suspendDevice()
    {
      deviceSuspended = true;
    }

    void resumeDevice()
    {
      deviceSuspended = false;
    }

    bool isActive()
    {
        return this->isActiveState;
    }
    void stopDevice()
    {
      QMetaObject::invokeMethod(this,"finishCommunication", Qt::QueuedConnection);
    }

    Q_INVOKABLE void finishCommunication();
private slots:
    void on_multiRadioButton_pressed();
    void on_singleRadioButton_pressed();
    void on_statusRequestRadioButton_released();
    void on_pingRadioButton_released();
    void on_startButton_clicked();
    void on_rssiButton_released();

    void on_delay_valueChanged(int value);

private:
    size_t calculateProperIndex(int i);
    void clearButtonsColor();
    bool compareCrc(uint8_t *data, const uint8_t packetType);
    void paintModule(PingAckStatus status, int16_t moduleNum);
    Ui::Feedback *ui;
    QGridLayout * mainLayout;
    //controller class to connect incoming packet data with GUI
    Controller* controller;
    QButtonGroup buttonsGroup;

    std::vector<std::unique_ptr<QPushButton>> buttonsVec;
    std::unique_ptr<QPlainTextEdit> logs;
    size_t colCount;
    size_t rowCount;

    size_t colCurrent;
    size_t rowCurrent;
    uint8_t sentSeqNumber;

    size_t currentModule;
    int packet_type;

    std::atomic<bool> isVerifyCalled, isActiveState, ethernetErrorOccured, deviceSuspended;

    std::vector<ModuleStatus> moduleStatuses;
};

#endif // PACKETTRANSFERVIEW_H
