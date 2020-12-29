#pragma once
#include <QMainWindow>

#include "modules_configuration.hpp"
#include "modules_verify.hpp"
#include "visual_debug.hpp"
#include "player.hpp"
#include "settings.hpp"

namespace Ui {
    class main_window;
}

namespace gui {

    class main_window : public QMainWindow
    {
        Q_OBJECT
    public:
        main_window(QWidget *parent = nullptr);
        ~main_window();

    private:
        Ui::main_window *ui;
        player player_widget;
        modules_configuration module_configuration_widget;
        modules_verify module_verify_widget;
        visual_debug visual_debug_widget;
        settings settings_widget;
    };
} // namespace gui
