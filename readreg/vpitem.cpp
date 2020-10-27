#include "readreg/vpitem.h"
#include "ui_vpitem.h"

//
vpItem::vpItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::vpItem) {
    ui->setupUi(this);

    ui->pushButton->setFixedHeight(ui->ledEn->height());
    ui->pushButton->setFixedWidth(2*ui->ledEn->height());

    setRegime(REGIME_connect);

    connect(ui->pushButton, &QPushButton::pressed,
            this, &vpItem::pressedBtn);

    connect(ui->pushButton, &QPushButton::released,
            this, &vpItem::pressedBtn);

    setFixedSize(sizeHint());
}

//
vpItem::~vpItem() {
    delete ui;
}

//
void
vpItem::setRegime(vpItem::regime_t regime) {
    ui->ledEn->setCheckable(true);
    ui->ledEn->setDisabled(true);

    ui->ledDs->setCheckable(true);
    ui->ledDs->setDisabled(true);
}

//
void
vpItem::setText(const QString &text) {
    ui->pushButton->setText(text);
}

//
void
vpItem::setLedEn(bool enable) {
    ui->ledEn->setChecked(enable);
}

//
void
vpItem::setLedDs(bool enable) {
    ui->ledDs->setChecked(enable);
}

//
void
vpItem::setBtn(bool enable) {
    QColor col = QColor(enable ? Qt::green : Qt::gray);

    if(col.isValid()) {
        QString qss = QString("background-color: %1").arg(col.name());
        ui->pushButton->setStyleSheet(qss);
    }

    ui->pushButton->setChecked(enable);
}

//
bool
vpItem::getBtn() const {

    return false;
}


