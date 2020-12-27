#include <QApplication>
#include <QtWidgets>
#include <QString>
#include <fmt/format.h>

#include "logger.hpp"

int
main(int argc, char* argv[])
{
  QApplication app(argc, argv);
  TLOG_INFO(MAJOR, "P.I.W.O Player");
  LOG(MAJOR, INFO, "Hello World!");
  return app.exec();
}
