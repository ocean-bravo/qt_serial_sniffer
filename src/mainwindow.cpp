#include "mainwindow.h"
#include "ui_mainwindow.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::udpMessage(QString str)
//{
//    ui->udpMessages->setTextColor(QColor("black"));

//    QString reText = ui->regExp1->text();
//    QRegExp rx(reText);

//    if (!reText.isEmpty() && str.contains(rx))
//    {
//        if (ui->cut1->isChecked())
//        {
//            str.remove(rx);
//        }
//        ui->udpMessages->setTextColor(QColor("blue"));
//    }

//    reText = ui->regExp2->text();
//    rx = QRegExp(reText);

//    if (!reText.isEmpty() && str.contains(rx))
//    {
//        if (ui->cut2->isChecked())
//        {
//            str.remove(rx);
//        }
//        ui->udpMessages->setTextColor(QColor("orange"));
//    }

//    reText = ui->regExp3->text();
//    rx = QRegExp(reText);

//    if (!reText.isEmpty() && str.contains(rx))
//    {
//        ui->udpMessages->setTextColor(QColor("red"));
//    }

//    reText = ui->regExp4->text();
//    rx = QRegExp(reText);

//    if (!reText.isEmpty() && str.contains(rx))
//    {
//        ui->udpMessages->setTextColor(QColor("green"));
//    }

//    // удаляю любое количество символов \n в конце
//    rx = QRegExp("[\\n]+$");
//    str.remove(rx);

//    if (str.isEmpty())
//        return;

//    if (ui->udpCurrentTime->isChecked())
//    {
//        str = QTime::currentTime().toString(Qt::ISODateWithMs) + "\t" + str;
//    }

//    ui->udpMessages->append(str);
//}

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
                ui->log->append(data.toHex());
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
                ui->log->append(data.toHex());
            });
        }
    }
}

