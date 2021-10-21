#include "combobox.h"

#include <QLineEdit>

TComboBox::TComboBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(true);
    lineEdit()->setReadOnly(true);
    lineEdit()->setAlignment(Qt::AlignRight);
    setFocusPolicy(Qt::NoFocus);
}

//
void TComboBox::showPopup()
{
    emit popuped();

    // TODO Проверить как это будет работать с виджетом
    // выбора последовательного порта
    if (count() > 1)
    {
        QComboBox::showPopup();
    }
}
