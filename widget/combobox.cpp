#include "combobox.h"

TComboBox::TComboBox(QWidget *parent) : QComboBox(parent) {

}

//
void
TComboBox::showPopup() {
  emit popuped();

  QComboBox::showPopup();
}
