#include "animation_progress.hpp"
#include "ui_animation_progress.h"

animation_progress::animation_progress(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::animation_progress)
{
  ui->setupUi(this);
}

animation_progress::~animation_progress()
{
  delete ui;
}

void
animation_progress::updateBar(int value)
{
  this->ui->progressBar->setValue(value);
}
