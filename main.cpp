#include <QApplication>

#include "main_window.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    gui::main_window w;
    w.show();
    return a.exec();
}
