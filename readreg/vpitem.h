#ifndef VPITEM_H
#define VPITEM_H

#include <QWidget>

namespace Ui {
class vpItem;
}

class vpItem : public QWidget {
    Q_OBJECT

public:
    enum regime_t {
        REGIME_offline = 0,
        REGIME_connect
    };

    explicit vpItem(QWidget *parent = nullptr);
    ~vpItem();

    void setRegime(regime_t regime);
    void setText(const QString &text);

public slots:
    void setLedEn(bool enable);
    void setLedDs(bool enable);
    void setBtn(bool enable);
    bool getBtn() const;

signals:
    void pressedBtn();

private:
    Ui::vpItem *ui;
    quint16 a;
};

#endif // VPITEM_H
