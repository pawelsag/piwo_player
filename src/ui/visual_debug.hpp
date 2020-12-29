#pragma once
#include <QWidget>

#include "display.hpp"

namespace Ui {
class visual_debug;
}
namespace gui {
  class visual_debug : public QWidget
  {
      Q_OBJECT

  public:
      explicit visual_debug(QWidget *parent = nullptr);
      void add_screen(int rows, int cols, int width, int height);
      ~visual_debug();

  private:
      Ui::visual_debug *ui;
      extension::display screen;
  };
} //namepspace gui
