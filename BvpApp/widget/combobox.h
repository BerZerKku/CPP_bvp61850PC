#ifndef TCOMBOBOX_HPP
#define TCOMBOBOX_HPP

#include <QComboBox>

class TComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit TComboBox(QWidget *parent = nullptr);

signals:
    void popuped();

private:
    void showPopup() override;
};

#endif  // TCOMBOBOX_HPP
