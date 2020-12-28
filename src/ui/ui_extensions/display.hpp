#ifndef DISPLAY_H
#define DISPLAY_H

#include <QtOpenGL/QGLWidget>
#include <QtWidgets>
namespace player{

class Display : public QGLWidget
{
  Q_OBJECT
public slots:
  void setColorInCell(uint row,
                      uint col,
                      unsigned char R,
                      unsigned char G,
                      unsigned char B);
  void setColorInCell(uint row, uint col, uint color);
  void setFrame(QImage& newFrame);
  void resizeEvent(QResizeEvent* event) override;
  void clearScreen();

public:
  explicit Display(uint x_size = 10,
                   uint y_size = 10);

  Q_INVOKABLE void displayResize(size_t x_size, size_t y_size);
  void createScene();
  void deleteScene();
  ~Display() override;

private:
  QGridLayout* mainLayout;
  // controller class to connect incoming packet data with GUI
  QImage currentFrame;
  QScrollArea* scrollArea;
  // X,Y axis of buttons
  uint32_t colCount, rowCount, width_, heigth_, sizeX, sizeY;

  void paintEvent(QPaintEvent* event) override;
};
}
#endif // DISPLAY_H
