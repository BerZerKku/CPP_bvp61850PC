#include "readreg.h"
#include <QDebug>
#include <QGridLayout>
#include <QLabel>
#include <QSpacerItem>


TReadReg::TReadReg(QWidget *parent) : QWidget(parent) {
  QHBoxLayout *layout = new QHBoxLayout();
  this->setLayout(layout);

  // FIXME Сейчас порядок жестко привязан к порядку в ENUM
  for(quint8 i = 0; i < rReg.size(); i++) {
    rReg.at(i).setGroup(static_cast<vpReg::group_t> (i));
  }

  for(auto &reg: rReg) {
    reg.setFixedSize(reg.sizeHint());
    reg.setRegime(vpItem::REGIME_connect);
    layout->addWidget(&reg);
  }

  setCom64(false);
}

//
void
TReadReg::setReg(vpReg::group_t group, TReadReg::regFunc_t func, quint16 value) {
  vpReg *item = &rReg.at(group);

  switch(func) {
    case REG_FUNC_LED_ENABLE: {
      item->setEnLeds(value);
    } break;
    case REG_FUNC_LED_DISABLE: {
      item->setDsLeds(value);
    } break;
    case REG_FUNC_BUTTON: {
      item->setBtns(value);
    } break;
  }
}

//
void
TReadReg::setCom64(bool enable) {
  rReg.at(vpReg::GROUP_com48to33).setVisible(enable);
  rReg.at(vpReg::GROUP_com64to49).setVisible(enable);
}

//
void
TReadReg::clear() {
  for(vpReg &reg: rReg) {
    reg.clear();
  }
}
