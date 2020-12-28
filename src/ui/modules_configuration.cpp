#include "modules_configuration.hpp"
#include "ui_modules_configuration.h"
namespace gui {
    modules_configuration::modules_configuration(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::modules_configuration)
    {
        ui->setupUi(this);
    }

    modules_configuration::~modules_configuration()
    {
        delete ui;
    }
} // namespace gui
