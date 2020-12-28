#include "configure_tx.hpp"
#include "ui_configure_tx.h"

configure_tx::configure_tx(QWidget* parent)
  : QDialog(parent)
  , ui(new Ui::configure_tx)
{
}

configure_tx::~configure_tx()
{
  delete ui;
}


void
configure_tx::refresh_config()
{
}

void
configure_tx::keyReleaseEvent(QKeyEvent* event)
{
}

void
configure_tx::on_deviceList_currentIndexChanged(int index)
{
}

void
configure_tx::showEvent(QShowEvent*)
{
}
