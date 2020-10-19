#include "serial.h"
#include "ui_serial.h"

TSerial::TSerial(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TSerial)
{
    ui->setupUi(this);
}

TSerial::~TSerial()
{
    delete ui;
}
