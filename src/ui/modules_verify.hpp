#ifndef MODULES_VERIFY_H
#define MODULES_VERIFY_H

#include <QWidget>

namespace Ui {
    class modules_verify;
}
namespace gui {
    class modules_verify : public QWidget
    {
        Q_OBJECT

    public:
        explicit modules_verify(QWidget *parent = nullptr);
        ~modules_verify();

    private:
        Ui::modules_verify *ui;
    };
} //namepspace gui
#endif // MODULES_VERIFY_H
