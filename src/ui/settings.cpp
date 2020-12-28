#include "settings.hpp"
#include "ui_settings.h"
namespace gui {
    settings::settings(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::settings)
    {
        ui->setupUi(this);
    }

    settings::~settings()
    {
        delete ui;
    }
} // namespace gui
