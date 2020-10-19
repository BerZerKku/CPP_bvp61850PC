#ifndef SERIAL_H
#define SERIAL_H

#include <QWidget>

namespace Ui {
class TSerial;
}

class TSerial : public QWidget
{
    Q_OBJECT

public:
    explicit TSerial(QWidget *parent = nullptr);
    ~TSerial();

private:
    Ui::TSerial *ui;
};

#endif // SERIAL_H
