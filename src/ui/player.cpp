#include "player.h"
#include "controller.h"
#include "helperclasses.h"
#include "time_convertion.h"
#include "misc.h"
#include "bass_audio/bass.h"
#include "json-develop/src/json.hpp"
#include "ui_player.h"

#include <QtConcurrent/QtConcurrent>
#include <chrono>
#include <fstream>
#include <fmt/format.h>
#include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;
using chrono_type = std::chrono::time_point<std::chrono::high_resolution_clock>;
using Time = std::chrono::high_resolution_clock;


Player::Player(QWidget* parent, Controller* controller)
  : QWidget(parent)
  , ui(new Ui::Player)
  , animationTable(parent, this)
{
  this->controller = controller;
  ui->setupUi(this);
  ui->TimeLine->setPlayer(this);
  animationTable.setMinimumWidth(300);
  animationTable.setMaximumWidth(300);
  ui->panelRight->addWidget(&animationTable);
  ui->TimeLine->setEnabled(false);
  playerScreen = std::make_unique<player::Display>(
    this->controller,
    static_cast<uint>(this->parentWidget()->size().width() -
                      (this->animationTable.size().width() + 60)),
    static_cast<uint>(this->parentWidget()->size().height() -
                      (this->ui->ControlBox->size().height() + 50)));

  ui->PlayerScreen->addWidget(playerScreen.get());

  connect(&this->animationTable,
          SIGNAL(doubleClicked(const QModelIndex&)),
          this,
          SLOT(onDoubleClicked(const QModelIndex&)));
  auto [rows, cols] = this->controller->getSceneSize();
  colCount = rows;
  rowCount = cols;

  qRegisterMetaType<QVector<int>>("QVector<int>");
}

Player::~Player()
{
  if (this->isActive()) {
    emit(on_StopButton_released());
  }
  delete ui;
}

void
Player::reset()
{

  this->animationTable.item(static_cast<int>(animationActiveId), 3)->setText("");
  this->ui->TimeLine->setValue(0);
  this->ui->TimeLine->setMaximum(
    static_cast<int>(activeAnimationVector[0]->frame_count() - 1));
  this->resetActiveStreams();

  animationActiveId = 0;
  frameActiveID = 0;
  frameActiveOffset = 0;
  elapsedAnimationTime = 0;
  // clear screen
  if (Controller::activeTxCount > 0)
    this->controller->clearScene();
  // TODO implemenet clearing scene from shared buffer 
  // else
  //  this->controller->clearSceneOffline();

  this->ui->TimeLabel->setText("00:00");

  QMetaObject::invokeMethod(this->ui->TimeLine,
                            "setEnabled",
                            Qt::QueuedConnection,
                            Q_ARG(bool, false));

  state = Player_T::STATE::IDLE;
}

void
Player::loadAnimmation()
{
  // set active frame number
  frameActiveID = 0;
  frameActiveOffset = 0;

  // clear screen
  if(Controller::activeTxCount > 0)
    this->controller->clearScene();
  // TODO clearing the scene will be implemented withing 
  // scene memory manager
  // else
  //   this->controller->clearSceneOffline();
  // *** reset neccesary values after rewind ***
  // *** and new anim load ***
  // *** set slider frame size for current anim ***
  this->ui->TimeLine->setMaximum(
    static_cast<int>(activeAnimationVector[animationActiveId]->frame_count() - 1));
  this->ui->TimeLine->setValue(frameActiveID);
  this->ui->TimeLabel->setText("00:00");
}

void
Player::resizeDevice(size_t x, size_t y)
{
  if(this->isActive())
    this->state = Player_T::STATE::STOPPED;

  // clear old anims
  this->activeAnimationVector.clear();
  this->animationTable.clear();

  // reset all settings
  this->animationTable.setRowCount(0);
  this->ui->TimeLabel->setText("00:00");

  QMetaObject::invokeMethod(
    ui->TimeLine, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, false));

  QMetaObject::invokeMethod(
    ui->TimeLine, "setValue", Qt::QueuedConnection, Q_ARG(int, 0));

  this->animationActiveId = 0;
  this->frameActiveID = 0;
  this->animationLoadedCounter = 0;
  // set new size
  this->colCount = x;
  this->rowCount = y;
  // set display to proper size
  this->playerScreen->displayResize(colCount, rowCount);
  // set new state
  this->state = Player_T::STATE::NODATA;
}


void
Player::on_key_delete(int idx)
{
  if (this->isActive() && animationLoadedCounter > 0)
    return;
  this->reset();

  this->activeAnimationVector.erase(this->activeAnimationVector.begin() + idx);
  animationLoadedCounter--;


  if (animationLoadedCounter == 0)
     state = Player_T::STATE::NODATA;
}

void
Player::on_change_anim_order(uint src, uint dst)
{
    this->activeAnimationVector[dst].swap(this->activeAnimationVector[src]);
}
#include<iostream>
void Player::addAnimation(const std::string &src)
{
    Player_T::Animation anim(src.c_str());
    switch(anim.status().error) {
      case piwo::animation::error_e::ok:
        break;
      case piwo::animation::error_e::cant_open_file:
        fmt::print("Cant open file {}\n", src);
        return;
      case piwo::animation::error_e::bad_signature:
        fmt::print("File {} has incorrect signature\n", src);
        return;
      case piwo::animation::error_e::bad_parse:
        fmt::print("File {} format incorrect\n", src);
        return;
      default:
        fmt::print("Unexpected error\n");
        return;
    }
    // check size of aniamtion
    // only when animation is bigger than actual size
    // change grid to bigger size
    if (anim.height() > rowCount || anim.width() > colCount) {
      QMessageBox mb(QMessageBox::Information, "Info",
                     QString("Animation %1 size is differnet from you player size configuration. Please match sizes to load this anim")
                       .arg(src.c_str())
                       .toStdString()
                       .c_str(),
                     QMessageBox::Ok);
      mb.exec();
      return;
    }
    activeAnimationVector.push_back(std::make_unique<Player_T::Animation>(std::move(anim)));

    createNewRowInTable(
      fs::path(src).filename().c_str(),
      activeAnimationVector[animationLoadedCounter]->total_time());
}


void
Player::on_LoadAnimation_set_released()
{
  this->loadAnimationConfig();
}

void
Player::on_SaveAnimation_set_released()
{
  if(this->animationLoadedCounter > 0)
    this->saveAnimationConfig();
}

void
Player::on_new_animations_appeared()
{
  std::string src;
  progressDialog.show();
  // call below method to process waiting events,
  // if we do not call this method,
  // the progress dialog will spawn after the animation
  // loading loop
  QCoreApplication::processEvents();
  auto animationCount = animationTable.pathList.size();
  for (uint i = 0; i < animationCount; i++) {

    src = animationTable.pathList[i].toStdString();
    addAnimation(src);
    progressDialog.updateBar((double(i) / animationCount) * 100);
    animationLoadedCounter++;
  }
  if (animationLoadedCounter > 0) {
    state = Player_T::STATE::IDLE;
  }
  progressDialog.hide();
}

void
Player::createNewRowInTable(const QString& name, size_t duration)
{
  size_t hours{ 0 }, minutes{ 0 }, seconds{ 0 };

  convertTime(duration, &hours, &minutes, &seconds);

  QString timeFormat = fmt::format("{:0>2}:{:0>2}:{:0>2}",hours,minutes,seconds).c_str();
  // append new anim to table
  animationTable.insertRow(animationLoadedCounter);
  animationTable.setItem(animationLoadedCounter,
                         1,
                         new QTableWidgetItem(name.toStdString().c_str()));
  animationTable.setItem(
    animationLoadedCounter, 2, new QTableWidgetItem(timeFormat));
  // assign active sign
  if (animationLoadedCounter == 0) {
    animationTable.setItem(0, 3, new QTableWidgetItem(QString("Active")));
    this->ui->TimeLine->setMaximum(
      static_cast<int>(activeAnimationVector[0]->frame_count() - 1));
  } else
    animationTable.setItem(
      animationLoadedCounter, 3, new QTableWidgetItem(QString("")));
}

void
Player::loadAnimationConfig()
{
  std::string filename, source;
  // new json parser
  json j;
  std::string line;
  auto filePath = Ui::FilePathManager::openFile<FILE_MANAGER::MODE::IMPORT_CONFIG>();
  if (filePath.length() == 0)
    return;
  // open file for reading
  std::ifstream is(filePath.toStdString().c_str());
  // check signature
  getline(is, line);

  if (line != "[PWIO7CONF]")
    return;

  while (getline(is, line)) {
    // parse it
    j = json::parse(line.c_str());

    source = j.at("source").get<std::string>();

    // this function fills our arrays with pixels, and whole duration time
    this->addAnimation(source);
  }

  if (animationLoadedCounter > 0) {
    state = Player_T::STATE::IDLE;
  }
}

bool
Player::saveAnimationConfig()
{
  // new json parser
  json j;

  auto filePath = Ui::FilePathManager::openFile<FILE_MANAGER::MODE::EXPORT_ANIMATION>();
  if (filePath.length() == 0)
    return false;

  std::ofstream os(filePath.toStdString().c_str(),
                   std::ofstream::out | std::ofstream::trunc);
  // write signature
  os << "[PWIO7CONF]\n";
  for (auto& animation : activeAnimationVector) {
    j["source"] = animation->animSrc().c_str();
    os << j.dump();
    os << "\n";
  }

  return true;
}



void Player::startMIDI(){
    qDebug() << "Midi started for id:" << this->animationActiveId <<" animation";
    this->controller->sendMidi<MIDI_SIGNAL::START>(this->animationActiveId);
    QMetaObject::invokeMethod(this->ui->TimeLine,
                              "setEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));
    QMetaObject::invokeMethod(this->ui->PauseButton,
                              "setEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, false));
}

void Player::stopMIDI(){
    qDebug() << "Midi stopped for id:" << this->animationActiveId <<" animation";
    this->controller->sendMidi<MIDI_SIGNAL::STOP>(this->animationActiveId);
    QMetaObject::invokeMethod(this->ui->TimeLine,
                              "setEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
    QMetaObject::invokeMethod(this->ui->PauseButton,
                              "setEnabled",
                              Qt::QueuedConnection,
                              Q_ARG(bool, true));
}

void
Player::prepareToSliderJump()
{
  if (this->state == Player_T::STATE::NODATA)
    return;
  this->state = Player_T::STATE::PAUSED;
  timerPressed = true;
}

void
Player::on_TimeLine_sliderReleased()
{
  if (state == Player_T::STATE::NODATA)
    return;

  timerPressed = false;
  state = Player_T::STATE::ACTIVE;
  if (activeAnimationVector[animationActiveId]->music())
    Controller::audioOut.play();

  animationTimeBase = std::chrono::system_clock::now();
}

void
Player::updateTimeLine()
{
  size_t minutes, seconds;

  // make progress on timeline
  this->ui->TimeLine->setValue(frameActiveID);
  convertTime(this->frameActiveID*MILISECONDS_PER_FRAME, &minutes, &seconds);
  // show elapsed time
  this->ui->TimeLabel->setText(
    fmt::format("{:0>2}:{:0>2}",minutes,seconds).c_str());
}

void
Player::on_TimeLine_valueChanged(int value)
{
  if (timerPressed == false)
    return;

  if (state == Player_T::STATE::NODATA) {
    return;
  }

  state = Player_T::STATE::PAUSED;
  size_t minutes{ 0 }, seconds{ 0 };
  this->frameActiveOffset = this->frameActiveID = value;

  for (uint i = currentAnimationRowCount, row = this->rowCount - 1;
       i <= currentAnimationRowCount;
       --i, --row) {
    for (size_t col = 0; col < activeAnimationVector[animationActiveId]->width();
         ++col) {
      // TODO implement frame rendering
      // player should use loaded frame and directly pass 
      // to packet creator
    }
  }

  this->controller->displayImage();
  (*activeAnimationVector[animationActiveId])[frameActiveID];
  if (activeAnimationVector[animationActiveId]->music()) {
    Controller::audioOut.pause();
    Controller::audioOut.setPosition(
      frameActiveID * MILISECONDS_PER_FRAME);
  }

  convertTime(static_cast<size_t>(frameActiveID * MILISECONDS_PER_FRAME), &minutes, &seconds);
  // show elapsed time
  this->ui->TimeLabel->setText(
    fmt::format("{:0>2}:{:0>2}",minutes,seconds).c_str());
}

void
Player::on_TimeLine_sliderPressed()
{
  timerPressed = true;
}

