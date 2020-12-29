#pragma once
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
    void add_screen(int rows, int cols, int width, int height);
    ~player();

  private:
    QButtonGroup buttons_group;
    QGridLayout* buttons_layout;
    Ui::player *ui;
    extension::display screen;

  };
} //namepspace gui
