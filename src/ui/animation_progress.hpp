#pragma once
#include <QDialog>

namespace Ui {
class animation_progress;
}

class animation_progress : public QDialog
{
  Q_OBJECT

public:
  explicit animation_progress(QWidget* parent = nullptr);
  ~animation_progress();
  void update_bar(int value);

private:
  Ui::animation_progress* ui;
};

