#pragma once

#include <QMap>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QScopedPointer>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:


signals:


private slots:


private:
    Ui::MainWindow* ui;
    void serviceMessage(const QString &msg);
    void openPorts();
    void closePorts();

    QMap<QString, QSerialPortInfo> _ports;

    QScopedPointer<QSerialPort> _port1;
    QScopedPointer<QSerialPort> _port2;
    void addMessage(const QString& source, const QByteArray& msg);
};
