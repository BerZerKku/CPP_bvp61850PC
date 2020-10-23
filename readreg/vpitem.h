#ifndef VPITEM_H
#define VPITEM_H

#include <QWidget>

namespace Ui {
class vpItem;
}

class vpItem : public QWidget {
    Q_OBJECT

public:
    explicit vpItem(QWidget *parent = nullptr);
    ~vpItem();

private:
    Ui::vpItem *ui;
};

#endif // VPITEM_H
