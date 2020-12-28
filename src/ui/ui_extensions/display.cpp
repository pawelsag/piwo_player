#include "display.hpp"
#include <QMouseEvent>
namespace player{

template<typename T>
constexpr void
UNUSED(T&& x) noexcept
{
  static_cast<void>(x);
}

Display::Display(uint x_size, uint y_size)
{
  // TODO get correct count of rows and cols
  int rows = 10, cols =12;
  this->rowCount = rows;
  this->colCount = cols;
  this->currentFrame = QImage(static_cast<int>(colCount),
                              static_cast<int>(rowCount),
                              QImage::Format_RGB32);
  this->currentFrame.fill(QColor(0, 0, 0));
  width_ = x_size;
  heigth_ = y_size;
  sizeX = width_ / colCount;
  sizeY = heigth_ / rowCount;
}

void
Display::paintEvent(QPaintEvent* event)
{
  UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::NonCosmeticDefaultPen);

  for (uint i = 0; i < colCount; i++) {
    for (uint j = 0; j < rowCount; j++) {
      {
        painter.fillRect(
          static_cast<int>(i * sizeX + 1),
          static_cast<int>(j * sizeY + 1),
          static_cast<int>(sizeX - 1),
          static_cast<int>(sizeY - 1),
          currentFrame.pixel(static_cast<int>(i), static_cast<int>(j)));
      }
    }
  }
  painter.end();
}

void
Display::createScene()
{
  this->currentFrame.fill(QColor(0, 0, 0));
  this->repaint();
}

void
Display::displayResize(size_t x_size, size_t y_size)
{

  colCount = x_size;
  rowCount = y_size;
  sizeX = width_ / colCount;
  sizeY = heigth_ / rowCount;
  this->currentFrame = QImage(static_cast<int>(colCount),
                              static_cast<int>(rowCount),
                              QImage::Format_RGB32);
  this->currentFrame.fill(QColor(0, 0, 0));
  this->repaint();
}

void
Display::setColorInCell(uint row,
                        uint col,
                        unsigned char R,
                        unsigned char G,
                        unsigned char B)
{

  if (row >= this->rowCount || col >= this->colCount)
    return;
  this->currentFrame.setPixelColor(
    static_cast<int>(col), static_cast<int>(row), QColor(R, G, B));
  this->repaint();
}

void
Display::setColorInCell(uint row, uint col, uint color)
{
  if (row >= this->rowCount || col >= this->colCount)
    return;
  this->currentFrame.setPixelColor(
    static_cast<int>(col), static_cast<int>(row), QColor(color));
}

void
Display::setFrame(QImage& newFrame)
{

  this->currentFrame = newFrame;
  this->repaint();
}

void
Display::resizeEvent(QResizeEvent* event)
{
  this->width_ = static_cast<size_t>(this->size().width());
  this->heigth_ = static_cast<size_t>(this->size().height());

  this->displayResize(this->colCount, this->rowCount);
  QWidget::resizeEvent(event);
}

void
Display::clearScreen()
{
  this->currentFrame.fill(QColor(0, 0, 0));
  this->repaint();
}

Display::~Display() {}
}
