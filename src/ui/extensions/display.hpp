#ifndef DISPLAY_H
#define DISPLAY_H

#include <QtOpenGL/QGLWidget>
#include <QtWidgets>

namespace gui::extension{

    class display : public QGLWidget
    {
      Q_OBJECT

    public:
      display()=default;
      display(uint rows,
              uint cols,
              uint width,
              uint height);
      display(display &&);
      display& operator=(display &&d);
      void create_scene();
      Q_INVOKABLE void display_resize(size_t x_size,
                                      size_t y_size);
      ~display() override;

    public slots:
      void set_color_cell(uint row,
                          uint col,
                          unsigned char R,
                          unsigned char G,
                          unsigned char B);
      void set_color_cell(uint row, uint col, uint color);
      void set_frame(QImage& newFrame);
      void resizeEvent(QResizeEvent* event) override;
      void clear_screen();

    private:
      // controller class to connect incoming packet data with GUI
      QImage current_frame;
      // X,Y axis of buttons
      uint32_t col_count, row_count, width_, heigth_, sizeX, sizeY;

      void paintEvent(QPaintEvent* event) override;
    };
} // namepsace gui::extension
#endif // DISPLAY_H
