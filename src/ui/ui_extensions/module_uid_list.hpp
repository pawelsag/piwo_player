#pragma once
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QtWidgets>

namespace m_ui {
  // list that contains all loaded raw uid
  // used in scene_manager
  class module_uid_list : public QListWidget
  {
    Q_OBJECT
  public:
    module_uid_list(QWidget* parent = nullptr)
      : QListWidget(parent)
    {
      setDragEnabled(true);
      setDefaultDropAction(Qt::MoveAction);
      setSelectionMode(QAbstractItemView::SingleSelection);
      setDragDropMode(QAbstractItemView::DragOnly);
    }
    // TODO chnage name of this function
    // and probably the implementation
    template<typename T>
    int indexOfSmallestValueGreaterThanCurrentValue(T arg)
    {
      for (int idx = 0; idx < this->count(); ++idx) {
        QListWidgetItem* item = this->item(idx);
        if (item->text().toUInt() > arg) {
          return idx;
        }
      }
      return this->count();
    }
  protected:
    void startDrag(Qt::DropActions supportedActions) override;
    Qt::DropAction supportedDropActions();
  };
} // namespace m_ui
