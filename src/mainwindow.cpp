#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace {

QString convertToAscii(const QByteArray& ba)
{
    QString converted;

    while (ba.size() > 0)
    {
        converted += QString(" %1").arg(ba.front());
    }

    return converted;
}

}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    ui->serial1->addItem("");
    ui->serial2->addItem("");

    for (const auto& portInfo : ports)
    {
        _ports.insert(portInfo.portName(), portInfo);
    }

    ui->serial1->addItems(_ports.keys());
    ui->serial2->addItems(_ports.keys());

    connect(ui->open, &QPushButton::clicked, this, &MainWindow::openPorts);
    connect(ui->close, &QPushButton::clicked, this, &MainWindow::closePorts);
    connect(ui->clear, &QPushButton::clicked, ui->log, &QTextEdit::clear);

    ui->log->setFont(QFont("Consolas"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addMessage(const QString& source, const QByteArray& msg)
{
    if (source == "1:")
        ui->log->setTextColor(QColor("navy"));

    if (source == "2:")
        ui->log->setTextColor(QColor("darkgreen"));

    ui->log->append(source + msg.toHex().toUpper());
    ui->log->append(source + convertToAscii(msg));
}

void MainWindow::serviceMessage(const QString& msg)
{
    ui->log->setTextColor(QColor("lightGrey"));
    ui->log->append(msg);
}

void MainWindow::closePorts()
{
    _port1.reset();
    _port2.reset();
    serviceMessage(QString("serial 1: port deleted"));
    serviceMessage(QString("serial 2: port deleted"));
}

void MainWindow::openPorts()
{
    _port1.reset(new QSerialPort(_ports[ui->serial1->currentText()]));
    _port2.reset(new QSerialPort(_ports[ui->serial2->currentText()]));

    _port1->setPortName(ui->serial1->currentText());
    _port2->setPortName(ui->serial2->currentText());

    _port1->setBaudRate(ui->baudrate->value());
    _port2->setBaudRate(ui->baudrate->value());

    _port1->setDataBits(QSerialPort::Data8);
    _port2->setDataBits(QSerialPort::Data8);

    _port1->setParity(QSerialPort::NoParity);
    _port2->setParity(QSerialPort::NoParity);

    _port1->setStopBits(QSerialPort::OneStop);
    _port2->setStopBits(QSerialPort::OneStop);

    {
        const bool ok = _port1->open(QIODevice::ReadWrite);

        if (!ok)
        {
            serviceMessage(QString("serial 1: try to open port \"%1\": %2").arg(ui->serial1->currentText()).arg(_port1->errorString()));
            _port1->clearError();
        }
        else
        {
            connect(_port1.data(), &QSerialPort::readyRead, [this]()
            {
                auto data = _port1->readAll();
                _port2->write(data);
                addMessage("1:", data);
            });
        }
    }

    {
        const bool ok = _port2->open(QIODevice::ReadWrite);

        if (!ok)
        {
            serviceMessage(QString("serial 2: try to open port \"%1\": %2").arg(ui->serial2->currentText()).arg(_port2->errorString()));
            _port2->clearError();
        }
        else
        {
            connect(_port2.data(), &QSerialPort::readyRead, [this]()
            {
                auto data = _port2->readAll();
                _port1->write(data);
                addMessage("2:", data);
            });
        }
    }
}

