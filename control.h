#ifndef CONTROL_H
#define CONTROL_H

#include <QWidget>
#include <QSerialPort>

namespace Ui {
class TControl;
}

class TControl : public QWidget
{
  Q_OBJECT

public:

  struct settings_t {
    uint32_t baud = QSerialPort::Baud19200;
    QSerialPort::DataBits dataBits = QSerialPort::Data8;
    QSerialPort::Parity parity = QSerialPort::NoParity;
    QSerialPort::StopBits stopBits = QSerialPort::TwoStop;
  };

  explicit TControl(QWidget *parent = nullptr);
  ~TControl();

  void setModbusConnection(bool enable);
  void setBspConnection(bool enable);
  settings_t getBspSettings() const;

public slots:
  void disableModbusSlot();
  void enableModbusSlot();
  void disableBspSlot();
  void enableBspSlot();

signals:
  void modbusStart();
  void modbusStop();
  void bspSettingsChanged();

private:
  Ui::TControl *ui;
  settings_t bspSettings;

  void setEnable(bool enable);
  void setEnableModbus(bool enable);

  void bspInit();

  void modbusInit();

private slots:
  void enableModbus();
  void disableModbus();
  void changeBsp(int index);
};

#endif // CONTROL_H
