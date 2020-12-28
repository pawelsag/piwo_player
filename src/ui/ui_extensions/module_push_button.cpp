#include "module_push_button.hpp"
#include <QMimeData>
#include "scene_manager.hpp"

namespace m_ui {
module_button::module_button(scene_manager* parent)
  : QPushButton(parent)
{
  this->parent = parent;
  setAcceptDrops(true);
}

module_button::module_button(const QString& text, scene_manager* parent)
  : QPushButton(text, parent)
{
  this->parent = parent;
  setAcceptDrops(true);
}

void
module_button::dropEvent(QDropEvent* event)
{
  if (event->mimeData()->hasText()) {
    const QMimeData* mime = event->mimeData();
    if (event->source() == this) {
      event->setDropAction(Qt::MoveAction);
      event->accept();
      emit changed(mime);
    } else {
      event->acceptProposedAction();
      emit changed(mime);
    }
  } else {
    event->ignore();
  }
  for (QWidget* widget : findChildren<QWidget*>()) {
    if (!widget->isVisible())
      widget->deleteLater();
  }
}
void
module_button::changed([[maybe_unused]]const QMimeData* mimeData)
{
  this->parent->on_button_clicked(this);
}

void
module_button::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasText()) {
    if (event->source() == this) {
      event->setDropAction(Qt::MoveAction);
      event->accept();
    } else {
      event->acceptProposedAction();
    }
  } else {
    event->ignore();
  }
}

} // namespace m_ui
