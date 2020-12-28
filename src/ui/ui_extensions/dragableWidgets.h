#ifndef DRAGABLEBUTTON_H
#define DRAGABLEBUTTON_H
#include <QDebug>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QtWidgets>

class UidManagment;
class Player;
// butons which contains user configuration
// used in uidManagment
class MyPushButton : public QPushButton
{
  Q_OBJECT
private:
  UidManagment* parentPtr;

public:
  MyPushButton(UidManagment* parent = nullptr);

  MyPushButton(const QString& text, UidManagment* parent = nullptr);
  void changed(const QMimeData* mimeData = nullptr);
  void clear();
  void setParent(UidManagment* parent) { this->parentPtr = parent; }

protected:
  void dropEvent(QDropEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
};
// list which contains all loaded raw uid
// used in uidManagment widget
class MyQListWidget : public QListWidget
{
  Q_OBJECT
public:
  MyQListWidget(QWidget* parent = nullptr)
    : QListWidget(parent)
  {
    setDragEnabled(true);
    this->setDefaultDropAction(Qt::MoveAction);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragDropMode(QAbstractItemView::DragOnly);
  }
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

// table which contains all loaded animations descriptions
// used in player widget
class AnimQTableWidget : public QTableWidget
{
  Q_OBJECT
public:
  AnimQTableWidget(QWidget* parent = nullptr, Player* playerPtr = nullptr);
  std::vector<QString> pathList;
  QList<QTableWidgetItem*> takeRow(int row);
  void setRow(int row, const QList<QTableWidgetItem*>& rowItems);
  void swapRows(int srcRow, int dstRow);
  Player* playerPtr;
public slots:
  void clear() {}
  void changed(const QMimeData* mimeData = nullptr);

  Q_INVOKABLE void doubleClicked(const QModelIndex& idx)
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
  bool internalDragEnable{ false };
};

bool
validateFileExtension(const char* file, size_t size);

#endif // DRAGABLEBUTTON_H
