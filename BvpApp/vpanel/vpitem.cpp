#include "vpanel/vpitem.h"
#include "ui_vpitem.h"

//
vpItem::vpItem(QWidget *parent) : QWidget(parent), ui(new Ui::vpItem)
{
    ui->setupUi(this);

    ui->pushButton->setFixedHeight(ui->ledEn->height());
    ui->pushButton->setFixedWidth(2 * ui->ledEn->height());

    ui->ledDs->setOnColor1(QColor(255, 255, 0));
    ui->ledDs->setOnColor2(QColor(192, 192, 0));
    ui->ledDs->setOffColor2(QColor(128, 128, 0));

    connect(ui->pushButton, &QPushButton::pressed, [=]() { pressedBtn(true); });

    connect(ui->pushButton, &QPushButton::released, [=]() { pressedBtn(false); });

    setRegime(REGIME_connect);

    setFixedSize(sizeHint());
}

//
vpItem::~vpItem()
{
    delete ui;
}

//
void vpItem::setRegime(vpItem::regime_t regime)
{
    ui->ledEn->setCheckable(true);
    ui->ledEn->setDisabled(true);

    ui->ledDs->setCheckable(true);
    ui->ledDs->setDisabled(true);

    ui->pushButton->setDisabled(true);
}

//
void vpItem::setText(const QString &text)
{
    ui->pushButton->setText(text);
}

//
void vpItem::btnEnabled(bool enable)
{
    ui->pushButton->setVisible(enable);
}

//
void vpItem::ledDsEnabled(bool enable)
{
    ui->ledDs->setVisible(enable);
}

//
void vpItem::ledEnEnabled(bool enable)
{
    ui->ledEn->setVisible(enable);
}

//
void vpItem::setLedEn(bool enable)
{
    ui->ledEn->setChecked(enable);
}

//
void vpItem::setLedDs(bool enable)
{
    ui->ledDs->setChecked(enable);
}

//
void vpItem::setBtn(bool enable)
{
    QString qss;
    QColor  col;

    if (enable)
    {
        qss = QString("background-color: %1").arg(QColor(Qt::green).name());
    }


    ui->pushButton->setStyleSheet(qss);
    ui->pushButton->setChecked(enable);
}

//
bool vpItem::getBtn() const
{

    return false;
}
