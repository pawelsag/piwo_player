#include "deploy_rx.hpp"

#include "deploy_rx.h"
#include <QDebug>

#define UNUSED(x) (void(x))

deploy_rx::deploy_rx(QWidget* parent)
  : QWidget(parent)
  , ui(new Ui::deploy_rx)
{
}

deploy_rx::~deploy_rx()
{
  delete ui;
}

void
deploy_rx::gridSelectionChanged()
{
}

void
deploy_rx::on_deleteTx_released()
{
}

void
deploy_rx::showEvent(QShowEvent* event)
{
}

void
deploy_rx::keyReleaseEvent(QKeyEvent* event)
{
}

void
deploy_rx::on_cleareAll_released()
{
}

void
deploy_rx::on_sendConfig_released()
{
}

void
deploy_rx::on_visibleRadioCheckBox_stateChanged(int mode)
{
}

void
deploy_rx::on_radio_id_currentIndexChanged(int index)
{
}

void
deploy_rx::on_channelComboBox_currentIndexChanged(int index)
{
}

void
deploy_rx::on_sendRadioButton_clicked()
{
}

void
deploy_rx::on_radioState_stateChanged(int state)
{
}
