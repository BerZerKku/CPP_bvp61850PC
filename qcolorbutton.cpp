#include "qcolorbutton.h"

QColorButton::QColorButton() {
  set(false);
}

void
QColorButton::set(bool enable) {
  setColor(enable ? Qt::darkGreen : Qt::gray);
}

void
QColorButton::setColor(Qt::GlobalColor color) {
  QColor col = QColor(color);
  if(col.isValid()) {
    QString qss = QString("background-color: %1").arg(col.name());
    setStyleSheet(qss);
  }
}
