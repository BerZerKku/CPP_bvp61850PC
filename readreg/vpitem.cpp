#include "readreg\vpitem.h"
#include "ui_vpitem.h"

vpItem::vpItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::vpItem) {
    ui->setupUi(this);

    setStyleSheet("border-style: solid; border-width: 1px; border-color: black;");
    ui->pushButton->setFixedWidth(ui->pushButton->height());
}

vpItem::~vpItem() {
    delete ui;
}
