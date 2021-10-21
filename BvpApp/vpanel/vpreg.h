#ifndef VPREG_H
#define VPREG_H

#include "vpanel/vpitem.h"
#include <QPushButton>
#include <QWidget>
#include <array>

namespace Ui
{
class vpReg;
}

class vpReg : public QWidget
{
    Q_OBJECT

    std::array<vpItem, 2 * CHAR_BIT> bits;

public:
    enum group_t
    {
        GROUP_com16to01 = 0,
        GROUP_com32to17,
        GROUP_com48to33,
        GROUP_com64to49,
        GROUP_control
    };

    enum regime_t
    {
        REGIME_offline = 0,
        REGIME_connect
    };

    explicit vpReg(QWidget *parent = nullptr);
    ~vpReg();

    void setGroup(group_t group);

    void setEnLeds(quint16 value);
    void setDsLeds(quint16 value);
    void setBtns(quint16 value);

    void clear();

public slots:
    void setRegime(vpItem::regime_t regime);

signals:
    void changedReg(int value);

private:
    Ui::vpReg *ui;

    void setGroupCom16to01();
    void setGroupCom32to17();
    void setGroupCom48to33();
    void setGroupCom64to49();
    void setGroupControl();

    void setupPushButton(QPushButton *pb, QString text, regime_t mode);

private slots:
    void pressedBtnSlot(int bit, bool checked);
    void changedEnLeds(int value);
    void changedDsLeds(int value);
    void changedBtns(int value);
};

#endif  // VPREG_H
