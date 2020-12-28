#ifndef MODULES_CONFIGURATION_H
#define MODULES_CONFIGURATION_H

#include <QWidget>

namespace Ui {
    class modules_configuration;
}

namespace gui {
    class modules_configuration : public QWidget
    {
        Q_OBJECT

    public:
        explicit modules_configuration(QWidget *parent = nullptr);
        ~modules_configuration();

    private:
        Ui::modules_configuration *ui;
    };
} //namepspace gui

#endif // MODULES_CONFIGURATION_H
