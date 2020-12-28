#pragma once
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QtWidgets>
#include <QPushButton>

namespace m_ui{
  class scene_manager;
  // butons that contains user configuration
  // used in scene_manager
  class module_button : public QPushButton
  {
    Q_OBJECT
  public:
    module_button(scene_manager* parent = nullptr);

    module_button(const QString& text, scene_manager* parent = nullptr);
    void changed(const QMimeData* mimeData = nullptr);
    void clear();
    void set_parent(scene_manager* parent) { this->parent = parent; }
  
  private:
    scene_manager* parent;

  protected:
    void dropEvent(QDropEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
  };
} // namespace m_ui
