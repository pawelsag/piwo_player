#include "dragableWidgets.h"
#include "player.h"
#include "uidmanagment.h"
#include <QMimeData>
#define UNSUED(a) ((void)a)
constexpr const char* PIWO_7_EXT = "piwo7";

MyPushButton::MyPushButton(UidManagment* parent)
  : QPushButton(parent)
{
  parentPtr = parent;
  setAcceptDrops(true);
}

MyPushButton::MyPushButton(const QString& text, UidManagment* parent)
  : QPushButton(text, parent)
{
  parentPtr = parent;
  setAcceptDrops(true);
}

void
MyPushButton::dropEvent(QDropEvent* event)
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
MyPushButton::changed(const QMimeData* mimeData)
{
  UNSUED(mimeData);
  parentPtr->on_button_clicked(this);
}

void
MyPushButton::dragEnterEvent(QDragEnterEvent* event)
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

void
MyQListWidget::startDrag([[maybe_unused]] Qt::DropActions supportedActions)
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
MyQListWidget::supportedDropActions()
{
  return Qt::MoveAction;
}

AnimQTableWidget::AnimQTableWidget(QWidget* parent, Player* playerPtr)
  : QTableWidget(parent)
{
  this->playerPtr = playerPtr;
  setAcceptDrops(true);
  setDragEnabled(true);
  setColumnCount(4);

  setHorizontalHeaderItem(1, new QTableWidgetItem("Title"));
  setHorizontalHeaderItem(2, new QTableWidgetItem("Duration"));
  setHorizontalHeaderItem(3, new QTableWidgetItem("Active"));
  setColumnHidden(0, true);

  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setEditTriggers(QAbstractItemView::NoEditTriggers);
  setDragDropMode(QAbstractItemView::DragDrop);
}

void
AnimQTableWidget::dropEvent(QDropEvent* event)
{
  if (event->mimeData()->hasText()) {
    // if draging internal elemnt
    if (internalDragEnable) {
      event->accept();
      int idxDst = this->indexAt(event->pos()).row();
      if (idxDst == -1)
        idxDst = rowCount() - 1;
      int idxSrc = event->mimeData()->text().toInt();
      this->playerPtr->on_change_anim_order(idxSrc, idxDst);
      // remove active indicator if set
      int ActiveId = this->playerPtr->getActiveAnimId();
      item(ActiveId, 3)->setText(" ");
      // choose direction ex. take first row and put on forth row
      swapRows(idxSrc, idxDst);
      item(ActiveId, 3)->setText("Active");
      return;
    }

    // if draging element from outside eg. widnows file
    const QMimeData* mime = event->mimeData();
    if (event->source() == this) {
      event->setDropAction(Qt::MoveAction);
      event->accept();

    } else {
      event->acceptProposedAction();
    }

    pathList.clear();
    QList<QUrl> urlList = mime->urls();
    // extract the local paths of the files
    for (int i = 0; i < urlList.size(); ++i) {
      if (validateFileExtension(
            urlList.at(i).fileName().toStdString().c_str(),
            static_cast<size_t>(
              urlList.at(i).fileName().toStdString().size()))) {
        pathList.emplace_back(urlList.at(i).toLocalFile());
      }
    }

    emit(changed());
  } else {
    event->ignore();
  }
}

void
AnimQTableWidget::changed(const QMimeData* mimeData)
{
  UNSUED(mimeData);

  playerPtr->on_new_animations_appeared();
}

void
AnimQTableWidget::dragEnterEvent(QDragEnterEvent* event)
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

void
AnimQTableWidget::dragMoveEvent(QDragMoveEvent* event)
{
  event->acceptProposedAction();
}

void
AnimQTableWidget::keyPressEvent(QKeyEvent* event)
{
  Player_T::STATE st = playerPtr->getState();
  if (st == Player_T::STATE::ACTIVE || st == Player_T::STATE::PAUSED ||
      st == Player_T::STATE::CONTINUE)
    return;
  event->accept();
  if (event->key() == Qt::Key_Delete) {
    if (rowCount()) {
      int idx = selectedItems().first()->row();
      playerPtr->on_key_delete(idx);
      removeRow(idx);

      setItem(0, 3, new QTableWidgetItem(QString("Active")));
    }
  }
}

void
AnimQTableWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
  event->accept();
}

void
AnimQTableWidget::startDrag([[maybe_unused]] Qt::DropActions supportedActions)
{
  if (playerPtr->getState() == Player_T::STATE::ACTIVE ||
      playerPtr->getState() == Player_T::STATE::PAUSED ||
      playerPtr->getState() == Player_T::STATE::CONTINUE)
    return;
  int row = currentRow();
  QMimeData* mimeData = new QMimeData;
  QByteArray ba;
  ba = QString::number(row).toLocal8Bit();
  mimeData->setText(QString::number(row));
  mimeData->setData("application/x-item", ba);
  QDrag* drag = new QDrag(this);
  drag->setMimeData(mimeData);
  internalDragEnable = true;
  drag->exec(Qt::MoveAction);
  internalDragEnable = false;
}

// takes and returns the whole row
QList<QTableWidgetItem*>
AnimQTableWidget::takeRow(int row)
{
  QList<QTableWidgetItem*> rowItems;
  for (int col = 0; col < columnCount(); ++col) {
    rowItems << takeItem(row, col);
  }
  return rowItems;
}

// sets the whole row
void
AnimQTableWidget::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
  for (int col = 0; col < columnCount(); ++col) {
    setItem(row, col, rowItems.at(col));
  }
}
void
AnimQTableWidget::swapRows(int srcRow, int dstRow)
{

  QList<QTableWidgetItem*> sourceItems = takeRow(srcRow);
  QList<QTableWidgetItem*> destinationItems = takeRow(dstRow);
  setRow(dstRow, sourceItems);
  setRow(srcRow, destinationItems);
}

bool
validateFileExtension(const char* file, size_t size)
{
  size_t pos = size;
  size_t signature_size = strlen(PIWO_7_EXT);
  for (size_t i = 0; i < size; i++) {
    if (file[i] == '.') {
      pos = i + 1;
      break;
    }
  }
  if (size - pos != signature_size)
    return false;

  int res = memcmp(file + pos, PIWO_7_EXT, signature_size);
  if (res == 0)
    return true;
  else
    return false;
}
