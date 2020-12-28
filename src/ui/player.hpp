#pragma once
#include "display.hpp"
#include "dragableWidgets.h"
#include "midi.h"
#include "animationprogressdialog.h"
#include "playertypes.h"

#include "iPlayerWidget.h"
#include <QWidget>

#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)
#define UNUSED(x) ((void)x)

using it_anim = std::vector<Player_T::Animation*>::iterator;
using chrono_time_point = std::chrono::time_point<std::chrono::system_clock>;

Q_DECLARE_METATYPE(QVector<int>)

namespace Ui {
class Player;
}

class Controller;

class Player : public QWidget, public iPlayerWidget
{
  enum class MODE
  {
    ONLINE,
    OFFLINE
  };
  Q_OBJECT
public:
  Player(QWidget* parent, Controller* controller);

  Q_INVOKABLE void resizeDevice(size_t x, size_t y);

  void on_new_animations_appeared();
  void on_key_delete(int idx);
  void on_change_anim_order(uint src, uint dst);

  void addAnimation(const std::string& src);
  void createNewRowInTable(const QString& name, size_t duration);
  void prepareToSliderJump();
  ~Player();
  animation_progress progressDialog;

private:
  Ui::Player* ui;
  Controller* controller;
  std::unique_ptr<player::Display> playerScreen;
  AnimQTableWidget animationTable;
  std::thread renderThread;
  uint8_t volumeLevel{ 50 };

  size_t colCount, rowCount;
  QString timeformat;

  // main container for imported animations
  std::vector<std::unique_ptr<Player_T::Animation>> activeAnimationVector;
  // player state
  std::atomic<Player_T::STATE> state{ Player_T::STATE::NODATA }, lastState;
  std::chrono::duration<double> diff;
  //
  chrono_time_point animationTimeBase = std::chrono::system_clock::now();
  chrono_time_point timeTimeCurrent = std::chrono::system_clock::now();
  // variables holds information
  // about currently displayed aniamtion and frame in animation
  std::atomic<uint32_t> animationActiveId{ 0 };
  std::atomic<uint32_t> frameActiveID{ 0 };
  std::atomic<uint32_t> frameActiveOffset{ 0 };
  std::atomic<uint32_t> animationLoadedCounter{ 0 };
  // time which elapsed from the begining of current animation
  int64_t elapsedAnimationTime{ 0 };
  size_t currentAnimationRowCount{ 0 };
  // some additional options
  std::atomic<bool> loopActive{ false }, shuffle{ false },
    timerPressed{ false };

  // buttons for media managment
  template<Player::MODE mode>
  void play();
  template<Player::MODE mode>
  void renderFrame();
  void renderSuspended();
  void frameLocker();
  void updateTimeLine();

  void reset();
  void loadAnimmation();
  bool saveAnimationConfig();
  void loadAnimationConfig();

  void startMIDI();

  void stopMIDI();
public:
  inline Player_T::STATE getState() const { return this->state; }

  inline uint32_t getActiveAnimId() { return this->animationActiveId; }


  inline bool isActive()
  {
    if (this->state == Player_T::STATE::ACTIVE ||
        this->state == Player_T::STATE::PAUSED)
      return true;
    return false;
  }
  void stopDevice(){
    QMetaObject::invokeMethod(this, "on_StopButton_released", Qt::QueuedConnection);
  }


  inline bool nextAnimation(){
    this->resetActiveStreams();
    if(this->animationActiveId == this->animationLoadedCounter - 1)
       return false;
     this->animationActiveId++;
     return true;
  }

  inline bool prevAnimation(){
      this->resetActiveStreams();
     if(this->animationActiveId == static_cast<decltype(this->animationActiveId)>(-1) )
       return false;
     this->animationActiveId--;
     return true;
  }

  inline void resetActiveStreams(){
    if (activeAnimationVector[animationActiveId]->music())
        Controller::audioOut.stop();
    else
      this->stopMIDI();
  }

signals:
  void vol_changed(int x);

public slots:
  void on_PlayButton_released();

  void on_PauseButton_released();

  void on_RewindLeft_released();

  void on_RewindRight_released();

  void on_StopButton_released();

  void on_Loop_toggled(bool checked);

  void on_LoadAnimation_set_released();

  void on_SaveAnimation_set_released();

  void onDoubleClicked(const QModelIndex&);

private slots:
  void on_VolumeSlider_valueChanged(int value);

  void on_TimeLine_sliderReleased();

  void on_TimeLine_valueChanged(int value);

  void on_TimeLine_sliderPressed();
};
