#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>

namespace Ui {
class TControl;
}

class TControl : public QWidget
{
    Q_OBJECT

public:
    explicit TControl(QWidget *parent = nullptr);
    ~TControl();

public slots:
    void disableSlot();
    void enableSlot();

signals:
    void readRegisters();
    void writeRegisters();
    void readAndWriteRegisters();

private:
    Ui::TControl *ui;

    void setEnable(bool enable);
};

#endif // CONTROL_H
