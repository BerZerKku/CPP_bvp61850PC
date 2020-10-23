#include "readreg/vpreg.h"
#include "ui_vpreg.h"

vpReg::vpReg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::vpReg)
{
    ui->setupUi(this);
}

vpReg::~vpReg()
{
    delete ui;
}
