#include "player.hpp"
#include "ui_player.h"
namespace gui {
    player::player(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::player)
    {
        ui->setupUi(this);
    }

    void
    player::configure_screen(int rows, int cols, int width, int height)
    {
        auto screen_width = static_cast<uint32_t>(width -
                                                (this->ui->tab_player_loaded_anims->size().width() + 50));
        auto screen_height = static_cast<uint32_t>(height - 150);

        screen = extension::display(rows, cols, screen_width, screen_height);
        this->ui->player_screen->addWidget(&screen);
    }


    player::~player()
    {
        delete ui;
    }
} // namespace gui
