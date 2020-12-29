#include "modules_configuration.hpp"
#include "ui_modules_configuration.h"
#include <string>
namespace gui {
  modules_configuration::modules_configuration(QWidget *parent, int rows, int cols) :
    QWidget(parent),
    ui(new Ui::modules_configuration)
  {
    this->ui->setupUi(this);
    this->buttons_layout =
        new QGridLayout(&this->buttons_widget);
    
    this->buttons_layout->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), rows, 0);
    this->buttons_layout->addItem(
      new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, cols, 0);
    this->fill_buttons(rows,cols);
    this->ui->scrl_module_area_2->setWidget(&this->buttons_widget);
  }

  void
  modules_configuration::fill_buttons(uint32_t rows, uint32_t cols)
  {
    size_t button_id;
    QPushButton* bt;

    for (int i = 0; i < rows; i++) {
      for (uint j = 0; j < cols; j++) {
        button_id = i * cols + j;
        bt = new QPushButton(std::to_string(button_id).c_str());
        bt->setFixedSize(120, 120);
        bt->setAutoFillBackground(true);

        this->buttons_group.addButton(
          bt,
          button_id); //<--add button to logical structure
        this->buttons_layout->addWidget(
          bt,
          static_cast<int>(i),
          static_cast<int>(j)); //<-- add buttons to visual layout
      }
    }
  }
  modules_configuration::~modules_configuration()
  {
      delete ui;
  }
} // namespace gui
