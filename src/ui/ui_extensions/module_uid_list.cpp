#include "module_uid_list.hpp"

#include <QMimeData>

namespace m_ui {
  void
  module_uid_list::startDrag([[maybe_unused]] Qt::DropActions supportedActions)
  {
    QListWidgetItem* item = currentItem();
    QMimeData* mimeData = new QMimeData;
    QByteArray ba;
    ba = item->text().toLocal8Bit();
    mimeData->setText(item->text());
    mimeData->setData("application/x-item", ba);
    QDrag* drag = new QDrag(this);
    item->setCheckState(Qt::Checked);
    drag->setMimeData(mimeData);
    if (!(drag->exec(Qt::MoveAction) == Qt::MoveAction)) {
      item->setCheckState(Qt::Unchecked);
    }
  }

  Qt::DropAction
  module_uid_list::supportedDropActions()
  {
    return Qt::MoveAction;
  }
} // namespace m_ui
