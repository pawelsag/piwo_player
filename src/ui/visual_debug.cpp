#include "testerwidget.h"
#include "helperclasses.h"
#include "payload.h"
#include "ui_testerwidget.h"

TesterWidget::TesterWidget(QWidget* parent, Controller* controller)
  : QWidget(parent)
  , ui(new Ui::TesterWidget)
{
  ui->setupUi(this);
  this->testSpeed = baseTestSpeed;
  this->choiceChange = false;
  this->testActive = false;
  this->testAudioActive = false;
  this->choiceAudioChange = false;
  this->deviceSuspended = false;

  this->controller = controller;
  auto [rows, cols] = this->controller->getSceneSize();
  this->colCount = cols;
  this->rowCount = rows;
  this->currentPalette.resize(this->colCount * this->rowCount);

  this->animationChoice = TEST_TYPE::KNIGHT_RIDER_VERTICAL;
  this->animationAudioChoice = AUDIO_TEST_TYPE::AUDIO_TEST_STATIC;

  this->primaryColor = Payload::Rgb::convertFromUint32(ColorPalette::BLACK);
  this->secondaryColor = Payload::Rgb::convertFromUint32(ColorPalette::WHITE);

  // apeend display to window
  // this is sheard display
  testerDisplay = std::make_unique<player::Display>(
    controller,
    static_cast<uint32_t>(this->parentWidget()->size().width() -
                          (this->ui->GroupRight->size().width() + 50)),
    static_cast<uint32_t>(this->parentWidget()->size().height() - 50));

  this->controller->registerWidget(testerDisplay.get());
  ui->LayoutLeft->addWidget(testerDisplay.get());
  // append tests labels to combobox
  ui->comboTest->addItem("Knight rider(V)");
  ui->comboTest->addItem("Knight rider(H)");
  ui->comboTest->addItem("Stroboscope(const Color)");
  ui->comboTest->addItem("Stroboscope(multi Color)");
  ui->comboTest->addItem("Random");
  ui->comboTest->addItem("Single button test");
  ui->comboTest->addItem("Color growth");
  // append Audio tests
  ui->AudioTypeCombo->addItem("Constant Color");
  ui->AudioTypeCombo->addItem("Gradient");
  ui->AudioTypeCombo->addItem("Green Yellow Red");
  //ui->AudioTypeCombo->addItem("Elipse");
  // append direction of test render
  ui->DirectionCombo->addItem("Bottom --> Top");
  ui->DirectionCombo->addItem("Top --> Bottom");
  ui->DirectionCombo->addItem("Left --> Right");
  ui->DirectionCombo->addItem("Right --> Left");
  // default settings
  // For constant color active Boxes are : Direction and Color
  ui->GradientGBox->hide();
  ui->VolumeGBox->hide();
  ui->SharpDynamicGBox->hide();

  // connect sliders with appropriate labels
  connect(this->ui->RedSlider,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(redValueChanged(int)));
  connect(this->ui->GreenSlider,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(greenValueChanged(int)));
  connect(this->ui->BlueSlider,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(blueValueChanged(int)));
  connect(this->ui->SpeedSlider,
          SIGNAL(valueChanged(int)),
          this,
          SLOT(speedValueChanged(int)));
  connect(this->ui->comboTest,
          SIGNAL(currentIndexChanged(int)),
          this,
          SLOT(testAnimationTypeChanged(int)));
  connect(this->ui->AudioTypeCombo,
          SIGNAL(currentIndexChanged(int)),
          this,
          SLOT(on_AudioTypeCombo_currentIndexChanged(int)));
  connect(this->ui->testBegin, SIGNAL(released()), this, SLOT(beginTest()));
  connect(this->ui->testEnd, SIGNAL(released()), this, SLOT(finishTest()));
}

void
TesterWidget::redValueChanged(int value)
{
  this->ui->RedValueLabel->setText(QString::number(value));
  primaryColor.setR(static_cast<uint8_t>(value));
  emit(this->changepallete());
}

void
TesterWidget::greenValueChanged(int value)
{
  this->ui->GreenValueLabel->setText(QString::number(value));
  primaryColor.setG(static_cast<uint8_t>(value));
  emit(this->changepallete());
}

void
TesterWidget::blueValueChanged(int value)
{
  this->ui->BlueValueLabel->setText(QString::number(value));
  primaryColor.setB(static_cast<uint8_t>(value));
  emit(this->changepallete());
}

void
TesterWidget::changepallete()
{
  if (this->animationChoice == TEST_TYPE::KNIGHT_RIDER_HORIZONTAL ||
      this->animationChoice == TEST_TYPE::KNIGHT_RIDER_VERTICAL)
    this->currentPalette.generatePalette<ColorManager::KNIGHT_PALETTE>(
      this->primaryColor, knightTailLength);

  this->currentPalette.savePrimaryColor(this->primaryColor);
}

void
TesterWidget::speedValueChanged(int value)
{
  this->ui->SpeedValueLabel->setText(QString::number(value));
  this->testSpeed = (baseTestSpeed / static_cast<uint8_t>(value));
}

void
TesterWidget::testAnimationTypeChanged(int value)
{
  TEST_TYPE v_type = static_cast<TEST_TYPE>(value);
  if (v_type == TEST_TYPE::RANDOM_STROBO ||
      v_type == TEST_TYPE::RANDOM_COLORS ||
      v_type == TEST_TYPE::PROGRESSIVE_GROWTH) {
    this->ui->RedSlider->setEnabled(false);
    this->ui->GreenSlider->setEnabled(false);
    this->ui->BlueSlider->setEnabled(false);
  } else {
    this->ui->RedSlider->setEnabled(true);
    this->ui->GreenSlider->setEnabled(true);
    this->ui->BlueSlider->setEnabled(true);
  }

  if (this->testActive != true) {
    this->animationChoice = static_cast<TEST_TYPE>(value);
    return;
  }
  this->animationChoice = v_type;
  this->choiceChange = true;
  this->finishTest();
  this->beginTest();
}

void
TesterWidget::resizeDevice(size_t x, size_t y)
{
  if (this->testActive == true)
    this->finishTest();
  if (this->testAudioActive == true)
    this->finishAudioTest();

  this->colCount = x;
  this->rowCount = y;
  this->testerDisplay->displayResize(x, y);
}

void
TesterWidget::beginTest()
{

  if (this->testActive == true || this->testAudioActive)
    return;
  if (this->controller->getActiveDevice() != nullptr)
    return;
  this->testActive = true;
  this->controller->registerWidget(testerDisplay.get());

  switch (animationChoice) {
    case TEST_TYPE::KNIGHT_RIDER_VERTICAL:
      this->currentPalette.generatePalette<ColorManager::KNIGHT_PALETTE>(
        this->primaryColor, knightTailLength);
      performTestAnim = std::thread(&TesterWidget::knightVerticalTest, this);
      break;
    case TEST_TYPE::KNIGHT_RIDER_HORIZONTAL:
      this->currentPalette.generatePalette<ColorManager::KNIGHT_PALETTE>(
        this->primaryColor, knightTailLength);
      performTestAnim = std::thread(&TesterWidget::knightHorizontalTest, this);
      break;
    case TEST_TYPE::CONST_STROBO:
      performTestAnim =
        std::thread(&TesterWidget::stroboscopeSingleColorTest, this);
      break;
    case TEST_TYPE::RANDOM_STROBO:
      performTestAnim =
        std::thread(&TesterWidget::stroboscopeMultiColorTest, this);
      break;
    case TEST_TYPE::RANDOM_COLORS:
      this->currentPalette.generatePalette<ColorManager::RANDOM_PALETTE>();
      performTestAnim = std::thread(&TesterWidget::randomColorsTest, this);
      break;
    case TEST_TYPE::CHECK_EACH_WINDOW:
      this->currentPalette.generatePalette<ColorManager::CONST_COLOR_PALETTE>(
        this->primaryColor);
      performTestAnim = std::thread(&TesterWidget::checkEachButtonTest, this);
      break;
    case TEST_TYPE::PROGRESSIVE_GROWTH:
      performTestAnim = std::thread(&TesterWidget::progressiveGrowthTest, this);
      break;
    default:
      break;
  }
}

void
TesterWidget::finishTest()
{
  if (this->testActive == true) {
    this->testActive = false;
    this->deviceSuspended = false;
    this->performTestAnim.join();
    this->testerDisplay->clearScreen();
    this->controller->clearDevicesBuffer();
  }
}

void
TesterWidget::on_AudioTypeCombo_currentIndexChanged(int index)
{
  this->animationAudioChoice = static_cast<AUDIO_TEST_TYPE>(index);

  switch (this->animationAudioChoice) {
    case AUDIO_TEST_TYPE::AUDIO_TEST_STATIC: // constant color
      ui->GradientGBox->hide();
      ui->VolumeGBox->hide();
      ui->SharpDynamicGBox->hide();
      ui->SingleColorGBox->show();
      break;
    case AUDIO_TEST_TYPE::AUDIO_TEST_GRADIENT: // gradient color
      ui->GradientGBox->show();
      ui->VolumeGBox->hide();
      ui->SharpDynamicGBox->hide();
      ui->SingleColorGBox->hide();

      break;
    case AUDIO_TEST_TYPE::AUDIO_TEST_GYR: // Green Yellow Red
      ui->GradientGBox->hide();
      ui->VolumeGBox->hide();
      ui->SharpDynamicGBox->hide();
      ui->SingleColorGBox->hide();
      break;
    case AUDIO_TEST_TYPE::ELIPSE: // ellipse
      ui->GradientGBox->show();
      ui->VolumeGBox->show();
      ui->SharpDynamicGBox->show();
      ui->SingleColorGBox->hide();
      break;
    default:
      break;
  }

  if (this->testAudioActive == false)
    return;

  this->choiceAudioChange = true;
  this->finishAudioTest();
  this->beginAudioTest();
}

void
TesterWidget::beginAudioTest()
{

  if (this->testAudioActive == true || this->testActive)
    return;
  if (this->controller->getActiveDevice() != nullptr)
    return;

  this->testAudioActive = true;
  this->controller->registerWidget(testerDisplay.get());

  switch (this->animationAudioChoice) {
    case AUDIO_TEST_TYPE::AUDIO_TEST_STATIC:
      // siingle color
      this->currentPalette.generatePalette<ColorManager::CONST_COLOR_PALETTE>(
        this->primaryColorFFT);
      performVisualisation =
        std::thread(&TesterWidget::AudioSingleColorBar, this);
      break;
    case AUDIO_TEST_TYPE::AUDIO_TEST_GRADIENT:
      // color Gradient
      this->primaryColorFFT =
        Payload::Rgb::convertFromUint32(ColorPalette::RED | ColorPalette::BLUE);
      this->secondaryColorFFT = Payload::Rgb::convertFromUint32(
        ColorPalette::GREEN | ColorPalette::BLUE);
      this->currentPalette.generatePalette<ColorManager::AUDIO_PALETTE>(
        this->primaryColorFFT, this->secondaryColorFFT, this->rowCount);
      performVisualisation =
        std::thread(&TesterWidget::AudioMultiColorBar, this);
      break;
    case AUDIO_TEST_TYPE::AUDIO_TEST_GYR:
      // color GreenYellowRed
      this->primaryColorFFT =
        Payload::Rgb::convertFromUint32(ColorPalette::RED);
      this->secondaryColorFFT =
        Payload::Rgb::convertFromUint32(ColorPalette::GREEN);
      this->currentPalette.generatePalette<ColorManager::AUDIO_PALETTE>(
        this->primaryColorFFT, this->secondaryColorFFT, this->rowCount);
      performVisualisation =
        std::thread(&TesterWidget::AudioMultiColorBar, this);
      break;
    default:
      break;
  }
}

void
TesterWidget::finishAudioTest()
{
  if (this->testAudioActive == true) {
    this->testAudioActive = false;
    this->deviceSuspended = false;
    this->performVisualisation.join();
    this->testerDisplay->clearScreen();
    this->controller->clearDevicesBuffer();
  }
}

void
TesterWidget::stopTests()
{
  this->finishTest();
  this->finishAudioTest();
}

void
TesterWidget::on_testBegin_2_released()
{
  if (this->controller->audioIn.isActive() == false)
    return;

  if (this->testAudioActive == true)
    return;

  this->controller->audioIn.resize(colCount + 1, rowCount);
  this->beginAudioTest();
}

void
TesterWidget::toggleRightPanel(DEVICE_INFO::INTERFACE_STATUS mode)
{
  if (mode == DEVICE_INFO::INTERFACE_STATUS::OFF)
    this->ui->GroupRight->setEnabled(false);
  else
    this->ui->GroupRight->setEnabled(true);
}

void
TesterWidget::on_testEnd_2_released()
{
  if (this->controller->audioIn.isActive() == false)
    return;

  this->finishAudioTest();
}

void
TesterWidget::on_AudioColorPick_released()
{
  QColorDialogUID colorInst;
  colorInst.PickColor();
  uint32_t color =
    QString(colorInst.getColor().cbegin() + 1).toUInt(nullptr, 16);
  // for audio test, we will use secondary color
  primaryColorFFT = Payload::Rgb::convertFromUint32(color);
  this->ui->AudioSingleColorLabel->setStyleSheet(QString("background-color: ") +
                                                 colorInst.getColor() + ";");
  this->currentPalette.generatePalette<ColorManager::CONST_COLOR_PALETTE>(
    this->primaryColorFFT);
}

void
TesterWidget::on_GradientColorBegin_released()
{
  QColorDialogUID colorInst;
  colorInst.PickColor();
  uint32_t color =
    QString(colorInst.getColor().cbegin() + 1).toUInt(nullptr, 16);
  primaryColorFFT = Payload::Rgb::convertFromUint32(color);

  QString colorFormula =
    QString("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, "
            "y2:0, stop:0 rgb(%1, %2, %3), stop:1 rgb(%4, %5, %6));")
      .arg(QString::number(primaryColorFFT.getR()),
           QString::number(primaryColorFFT.getG()),
           QString::number(primaryColorFFT.getB()),
           QString::number(secondaryColorFFT.getR()),
           QString::number(secondaryColorFFT.getG()),
           QString::number(secondaryColorFFT.getB()));

  this->ui->GradienLabel->setStyleSheet(colorFormula);
  this->currentPalette.generatePalette<ColorManager::AUDIO_PALETTE>(
    this->primaryColorFFT, this->secondaryColorFFT, this->rowCount);
}

void
TesterWidget::on_GradientColorEnd_released()
{
  QColorDialogUID colorinst;
  colorinst.PickColor();

  uint32_t color =
    QString(colorinst.getColor().cbegin() + 1).toUInt(nullptr, 16);
  this->secondaryColorFFT = Payload::Rgb::convertFromUint32(color);

  QString colorFormula =
    QString("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, "
            "y2:0, stop:0 rgb(%1, %2, %3), stop:1 rgb(%4, %5, %6));")
      .arg(QString::number(primaryColorFFT.getR()),
           QString::number(primaryColorFFT.getG()),
           QString::number(primaryColorFFT.getB()),
           QString::number(secondaryColorFFT.getR()),
           QString::number(secondaryColorFFT.getG()),
           QString::number(secondaryColorFFT.getB()));

  this->ui->GradienLabel->setStyleSheet(colorFormula);
  this->currentPalette.generatePalette<ColorManager::AUDIO_PALETTE>(
    this->primaryColorFFT, this->secondaryColorFFT, this->rowCount);
}

TesterWidget::~TesterWidget()
{
  stopTests();
  delete ui;
}
