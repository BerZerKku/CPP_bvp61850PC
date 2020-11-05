#include "paramtree.h"
#include <QLineEdit>
#include <QHeaderView>

//
TParamTree::TParamTree(QWidget *parent) :
  QTreeWidget (parent) {
  headerItem()->setText(0, "Параметр");
  headerItem()->setText(1, "Значение R/W");

  crtGroupVp();
  crtGroupCtrl();
  crtGroupError();
  crtGroupErrorRemote();

  expandAll();

  header()->resizeSections(QHeaderView::ResizeToContents);
  header()->setSectionResizeMode(0, QHeaderView::Fixed);
  header()->resizeSection(0, header()->sectionSize(0) + 5);
  setFixedWidth(static_cast<int> (1.75*header()->sectionSize(0)));

  setSelectionMode(QAbstractItemView::NoSelection);
  setFocusPolicy(Qt::NoFocus);


}

//
void
TParamTree::updateParameters() {
  for(BVP::param_t param: mapItems.keys()) {
    updateParameter(param);
  }
}

//
void
TParamTree::crtGroupCtrl() {
  QTreeWidgetItem *top = new QTreeWidgetItem();
  top->setText(0, "Управление");

  crtItem(top, BVP::PARAM_control, "Сигналы управления");

  insertTopLevelItem(topLevelItemCount(), top);
}

//
void
TParamTree::crtGroupError() {
  QTreeWidgetItem *top = new QTreeWidgetItem();
  top->setText(0, "Неисправности");

  crtItem(top, BVP::PARAM_error, "Флаг наличия неисправности");
  crtItem(top, BVP::PARAM_warning, "Флаг наличия предупреждения");
  crtItem(top, BVP::PARAM_defError, "Неисправности Защиты");
  crtItem(top, BVP::PARAM_defWarning, "Предупреждения Защиты");
  crtItem(top, BVP::PARAM_prmError, "Неисправности Приемника");
  crtItem(top, BVP::PARAM_prmWarning, "Педупреждения Приемника");
  crtItem(top, BVP::PARAM_prdError, "Неисправности Передатчика");
  crtItem(top, BVP::PARAM_prdWarning, "Педупреждения Передатчика");
  crtItem(top, BVP::PARAM_glbError, "Неисправности Общие");
  crtItem(top, BVP::PARAM_glbWarning, "Педупреждения Общие");

  insertTopLevelItem(topLevelItemCount(), top);
}

//
void
TParamTree::crtGroupErrorRemote() {
  QTreeWidgetItem *top = new QTreeWidgetItem();
  top->setText(0, "Неисправности Удаленного");

  crtItem(top, BVP::PARAM_defRemoteError, "Неисправности Защиты");
  crtItem(top, BVP::PARAM_prmRemoteError, "Неисправности Приемника");
  crtItem(top, BVP::PARAM_prdRemoteError, "Неисправности Передатчика");
  crtItem(top, BVP::PARAM_glbRemoteError, "Неисправности Общие");

  insertTopLevelItem(topLevelItemCount(), top);
}

//
void
TParamTree::crtGroupVp() {
  QTreeWidgetItem *top = new QTreeWidgetItem();
  top->setText(0, "Панель виртуальных ключей");

  crtItem(top, BVP::PARAM_dirControl, "Управление SAC2");
  crtItem(top, BVP::PARAM_blkComPrmAll, "Приемник SAC1");
  crtItem(top, BVP::PARAM_blkComPrmDir, "Блк. направлений SAnn.x");
  crtItem(top, BVP::PARAM_blkComPrm32to01, "Блк. команд приемника 32..01");
  crtItem(top, BVP::PARAM_blkComPrm64to33, "Блк. команд приемника 64..33");
  crtItem(top, BVP::PARAM_blkComPrd32to01, "Блк. команд передатчика 32..01");
  crtItem(top, BVP::PARAM_blkComPrd64to33, "Блк. команд передатчика 64..33");

  insertTopLevelItem(topLevelItemCount(), top);
}

//
void
TParamTree::crtItem(QTreeWidgetItem* top, BVP::param_t param, QString name) {
  QTreeWidgetItem *item = new QTreeWidgetItem();
  QLineEdit *lineedit = new QLineEdit();

  item->setText(0, name);

  lineedit->setReadOnly(true);
  lineedit->setFocusPolicy(Qt::NoFocus);
  lineedit->setStyleSheet("border: none");

  top->addChild(item);
  setItemWidget(item, 1, lineedit);
  mapItems.insert(param, item);
}

//
void
TParamTree::updateParameter(BVP::param_t param) {
  bool ok;

  BVP::TParam *p = BVP::TParam::getInstance();
  QTreeWidgetItem *item = mapItems.value(param);
  QLineEdit *lineedit = static_cast<QLineEdit *> (itemWidget(item, 1));

  uint32_t rvalue = p->getValueR(param);
  uint32_t wvalue = p->getValueW(param);

  lineedit->setText(QString("%1 / %2").
                    arg(rvalue, 8, 16, QLatin1Char('0')).
                    arg(wvalue, 8, 16, QLatin1Char('0')));
}
