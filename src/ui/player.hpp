#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>

#include"display.hpp"

namespace Ui {
    class player;
}
namespace gui {
    class player : public QWidget
    {
        Q_OBJECT

    public:
        explicit player(QWidget *parent = nullptr);
        void configure_screen(int rows, int cols, int width, int height);
        ~player();

    private:
        Ui::player *ui;
        extension::display screen;
    };
} //namepspace gui

#endif // PLAYER_H
