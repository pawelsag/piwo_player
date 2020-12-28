#ifndef TESTERWIDGET_H
#define TESTERWIDGET_H
#include "controller.h"
#include "colormanager.h"
#include "display.h"
#include "iPlayerWidget.h"
#include <chrono>

#include <QWidget>

namespace Ui {
class TesterWidget;
}

enum class TEST_TYPE
{
  KNIGHT_RIDER_VERTICAL = 0,
  KNIGHT_RIDER_HORIZONTAL = 1,
  CONST_STROBO = 2,
  RANDOM_STROBO = 3,
  RANDOM_COLORS = 4,
  CHECK_EACH_WINDOW = 5,
  PROGRESSIVE_GROWTH = 6
};

enum class AUDIO_TEST_TYPE
{
  AUDIO_TEST_STATIC = 0,
  AUDIO_TEST_GRADIENT = 1,
  AUDIO_TEST_GYR = 2,
  ELIPSE = 3
};

enum class KNIGHT_DIRECTION
{
  KNIGHT_LEFT,
  KNIGHT_RIGHT
};

class TesterWidget : public QWidget, public iPlayerWidget
{
  std::chrono::milliseconds THREAD_RELIEF_TIME {3};
  Q_OBJECT
  constexpr static auto baseTestSpeed = 1200;
  constexpr static auto knightTailLength = 5;

public:
  TesterWidget(QWidget* parent, Controller* controller);
  // finish currently active test
  Q_INVOKABLE void stopTests();
  void getRange();
  ~TesterWidget();

  // interface
  bool isActive()
  {
    return this->testActive == true || this->testAudioActive == true;
  }

  Q_INVOKABLE void resizeDevice(size_t x, size_t y);

  void suspendDevice()
  {

  }
  void resumeDevice()
  {

  }

  void stopDevice(){
    QMetaObject::invokeMethod(this, "stopDevice", Qt::QueuedConnection);
  }

private:
  // main display
  std::unique_ptr<player::Display> testerDisplay;
  Controller* controller;
  Ui::TesterWidget* ui;
  std::thread performTestAnim, performVisualisation;
  // image managment
  QPainter paint;

  // variables holds information about currnt speed, maximal point of top-left
  // and bottom-right
  unsigned int testSpeed;
  uint colCount, rowCount;

  TEST_TYPE animationChoice;
  AUDIO_TEST_TYPE animationAudioChoice;

  std::atomic<bool> testActive, choiceChange, testAudioActive,
    choiceAudioChange, deviceSuspended;
  // scondary color is necessary only to generate color range
  Payload::Rgb primaryColor, secondaryColor, primaryColorFFT{},
    secondaryColorFFT{};
  // palete to hold colors for Knight Test
  ColorManager currentPalette;

  Payload::Rgb BLACKCOLOR;
  // ###########
  // set of random animations for testing
  void knightVerticalTest();
  void knightHorizontalTest();
  void stroboscopeSingleColorTest();
  void stroboscopeMultiColorTest();
  void randomColorsTest();
  void checkEachButtonTest();
  void progressiveGrowthTest();

  /// ################################
  /// set of audio vizualization tests
  /// ################################
  void AudioSingleColorBar();
  void AudioMultiColorBar();
  void AudioElipseBar();
  /// ############################################

public slots:
  // standalone signals
  void redValueChanged(int value);
  void greenValueChanged(int value);
  void blueValueChanged(int value);
  void speedValueChanged(int value);
  void testAnimationTypeChanged(int);
  void changepallete();
  void toggleRightPanel(DEVICE_INFO::INTERFACE_STATUS mode);

private slots:
  void on_AudioTypeCombo_currentIndexChanged(int index);

  void on_testBegin_2_released();

  void on_testEnd_2_released();

  void beginTest();
  void finishTest();
  void beginAudioTest();
  void finishAudioTest();

  void on_AudioColorPick_released();

  void on_GradientColorBegin_released();

  void on_GradientColorEnd_released();
};

#endif // TESTERWIDGET_H
