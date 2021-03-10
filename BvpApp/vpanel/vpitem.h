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

  void btnEnabled(bool enable);
  void ledDsEnabled(bool enable);
  void ledEnEnabled(bool enable);

public slots:
  void setLedEn(bool enable);
  void setLedDs(bool enable);
  void setBtn(bool enable);
  bool getBtn() const;

signals:
  void pressedBtn(bool checked);

private:
  Ui::vpItem *ui;
};

#endif // VPITEM_H
