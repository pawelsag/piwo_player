#include "main_window.hpp"
#include "ui_main_window.h"
namespace gui {
  main_window::main_window(QWidget *parent)
      : QMainWindow(parent)
      , ui(new Ui::main_window)
  {
      ui->setupUi(this);
      this->ui->main_tab_widget->addTab(&this->module_configuration_widget, "Configure modules");
      this->ui->main_tab_widget->addTab(&this->module_verify_widget, "Verify modules");
      this->ui->main_tab_widget->addTab(&this->visual_debug_widget, "Visual debug");
      this->ui->main_tab_widget->addTab(&this->player_widget, "Player");
      this->ui->main_tab_widget->addTab(&this->settings_widget, "Settings");

      this->visual_debug_widget.configure_screen(10, 12, this->size().width(), this->size().height());
      this->player_widget.configure_screen(10, 12, this->size().width(), this->size().height());
  }

  main_window::~main_window()
  {
      delete ui;
  }
} // namespace gui
