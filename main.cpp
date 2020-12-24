#include <QApplication>
#include <QtWidgets>
#include <fmt/format.h>

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  fmt::print("Hello World!\n");
  return app.exec();
}
