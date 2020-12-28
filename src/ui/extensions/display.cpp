#include "display.hpp"
#include <QMouseEvent>
namespace gui::extension{

display::display(uint rows, uint cols, uint width, uint height)
{
  this->row_count = rows;
  this->col_count = cols;
  this->current_frame = QImage(static_cast<int>(col_count),
                              static_cast<int>(row_count),
                              QImage::Format_RGB32);
  this->current_frame.fill(QColor(0, 0, 0));
  this->width_ = width;
  this->heigth_ = height;
  sizeX = width_ / col_count;
  sizeY = heigth_ / row_count;
}

display::display(display &&d)
{
    this->row_count = d.row_count;
    this->col_count = d.col_count;
    this->current_frame = std::move(d.current_frame);
    this->current_frame.fill(QColor(0, 0, 0));
    width_ = d.width();
    heigth_ = d.height();
    sizeX = d.sizeX;
    sizeY = d.sizeY;
}

display&
display::operator=(display&& d)
{
    this->row_count = d.row_count;
    this->col_count = d.col_count;
    this->current_frame = std::move(d.current_frame);
    this->current_frame.fill(QColor(0, 0, 0));
    width_ = d.width();
    heigth_ = d.height();
    sizeX = d.sizeX;
    sizeY = d.sizeY;
    return *this;
}

void
display::paintEvent([[maybe_unused]] QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::NonCosmeticDefaultPen);

  for (uint i = 0; i < col_count; i++) {
    for (uint j = 0; j < row_count; j++) {
      {
        painter.fillRect(
          static_cast<int>(i * sizeX + 1),
          static_cast<int>(j * sizeY + 1),
          static_cast<int>(sizeX - 1),
          static_cast<int>(sizeY - 1),
          current_frame.pixel(static_cast<int>(i), static_cast<int>(j)));
      }
    }
  }
  painter.end();
}

void
display::create_scene()
{
  this->current_frame.fill(QColor(0, 0, 0));
  this->repaint();
}

void
display::display_resize(size_t x_size, size_t y_size)
{

  col_count = x_size;
  row_count = y_size;
  sizeX = width_ / col_count;
  sizeY = heigth_ / row_count;
  this->current_frame = QImage(static_cast<int>(col_count),
                              static_cast<int>(row_count),
                              QImage::Format_RGB32);
  this->current_frame.fill(QColor(0, 0, 0));
  this->repaint();
}

void
display::set_color_cell(uint row,
                        uint col,
                        unsigned char R,
                        unsigned char G,
                        unsigned char B)
{

  if (row >= this->row_count || col >= this->col_count)
    return;
  this->current_frame.setPixelColor(
    static_cast<int>(col), static_cast<int>(row), QColor(R, G, B));
  this->repaint();
}

void
display::set_color_cell(uint row, uint col, uint color)
{
  if (row >= this->row_count || col >= this->col_count)
    return;
  this->current_frame.setPixelColor(
    static_cast<int>(col), static_cast<int>(row), QColor(color));
}

void
display::set_frame(QImage& newFrame)
{

  this->current_frame = newFrame;
  this->repaint();
}

void
display::resizeEvent(QResizeEvent* event)
{
  this->width_ = static_cast<size_t>(this->size().width());
  this->heigth_ = static_cast<size_t>(this->size().height());

  this->display_resize(this->col_count, this->row_count);
  QWidget::resizeEvent(event);
}

void
display::clear_screen()
{
  this->current_frame.fill(QColor(0, 0, 0));
  this->repaint();
}

display::~display() {}
}
