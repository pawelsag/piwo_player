#include "modules_verify.hpp"
#include "ui_modules_verify.h"
namespace gui {
    modules_verify::modules_verify(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::modules_verify)
    {
        ui->setupUi(this);
    }

    modules_verify::~modules_verify()
    {
        delete ui;
    }
} // namespace gui
