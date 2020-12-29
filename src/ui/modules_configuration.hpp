#pragma once
#include <QWidget>
#include <QButtonGroup>
#include <QGridLayout>
#include "player_config.hpp"


namespace Ui {
    class modules_configuration;
}

namespace gui {
  class modules_configuration : public QWidget
  {
      Q_OBJECT

  public:
      explicit modules_configuration(QWidget *parent = nullptr, int rows=0, int cols=0);
      void fill_buttons(uint32_t rows, uint32_t cols);
      ~modules_configuration();

  private:
    QWidget buttons_widget;
    QButtonGroup buttons_group;
    QGridLayout* buttons_layout;
    Ui::modules_configuration *ui;
  };
} //namepspace gui

