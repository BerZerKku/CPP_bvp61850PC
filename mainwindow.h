#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

    const uint8_t deviceAddress = 0x0A;
    static const uint8_t comReadHoldingRegisters;
    static const uint8_t comWriteMultipleRegisters;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    uint16_t getUInt16(QVector<uint8_t> &pkg);
    void writePkg(QVector<uint8_t> &pkg);

private slots:
    void readRegistersSlot();
    void writeRegistersSlot();
    void readAndWriteRegistersSlot();

    void readSlot(int value);
};
#endif // MAINWINDOW_H
