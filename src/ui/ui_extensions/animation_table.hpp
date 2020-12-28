#pragma once
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QtWidgets>
namespace m_ui {
  class player;
  // table that contains all loaded animations descriptions
  // used in player widget
  class animation_table : public QTableWidget
  {
    Q_OBJECT
  public:
    animation_table(QWidget* parent = nullptr, player* player = nullptr);
    std::vector<QString> path_list;
    QList<QTableWidgetItem*> take_row(int row);
    void set_row(int row, const QList<QTableWidgetItem*>& row_items);
    void swap_rows(int f_row, int s_row);
    player* player;
  public slots:
    void clear() {}
    void changed(const QMimeData* mimeData = nullptr);
    void doubleClicked(const QModelIndex& idx)
    {
      QTableWidget::doubleClicked(idx);
    }
  protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void startDrag(Qt::DropActions supportedActions) override;

  private:
    bool drag_enable{ false };
  };
}
