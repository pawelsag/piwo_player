#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>

namespace Ui {
class settings;
}
namespace gui {
    class settings : public QWidget
    {
        Q_OBJECT

    public:
        explicit settings(QWidget *parent = nullptr);
        ~settings();

    private:
        Ui::settings *ui;
    };
} //namepspace gui
#endif // SETTINGS_H
