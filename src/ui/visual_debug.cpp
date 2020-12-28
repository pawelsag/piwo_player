#include "visual_debug.hpp"
#include "ui_visual_debug.h"
namespace gui {
    visual_debug::visual_debug(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::visual_debug)
    {
        ui->setupUi(this);
    }
    void
    visual_debug::configure_screen(int rows, int cols, int width, int height)
    {
        auto screen_width = static_cast<uint32_t>(width -
                                                (this->ui->gb_standard->size().width() + 50));
        auto screen_height = static_cast<uint32_t>(height - 50);

        screen = extension::display(rows, cols, screen_width, screen_height);
        this->ui->visual_debug_screen->addWidget(&screen);
    }

    visual_debug::~visual_debug()
    {
        delete ui;
    }
} // namespace gui
