#include <QButtonGroup>
#include "mainwindow.h"
#include "ui_mainwindow.h"

/**
 * @brief Constructor for the MainWindow class.
 *
 * Initializes the main window, sets up connections, and initializes member variables.
 *
 * @param parent The parent widget (default is nullptr)
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);
    makeRadioGroups();
    // Connect signals and slots
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::handleReadyRead);
    connect(ui->ComButton, &QPushButton::clicked, this, &MainWindow::connectCom);
    connect(ui->ComDisButton, &QPushButton::clicked, this, &MainWindow::closeCom);
    connect(ui->ButtonDongle1, &QPushButton::clicked, this, &MainWindow::dataDongle1);
    connect(ui->ButtonDongle2, &QPushButton::clicked, this, &MainWindow::dataDongle2);
    connect(ui->ButtonDongle3, &QPushButton::clicked, this, &MainWindow::dataDongle3);
    connect(ui->ButtonCouple, &QPushButton::clicked, this, &MainWindow::connectDongle);
}

/**
 * @brief Destructor for the MainWindow class.
 */
MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleReadyRead()
{
    QByteArray data = serial->readAll();
    QString searchString1 = "3";
    QString searchString2 = "4";
    QString searchString3 = "5";
    QString searchString4 = "6";
    QString searchString5 = "7";
    QString searchString6 = "8";
    if (data.contains(searchString1.toUtf8()) && one == 1) {
        ui->ledStatusDongle1->setStyleSheet("background-color: yellow;");
    }
    else if (data.contains(searchString2.toUtf8())) {
        ui->ledStatusDongle1->setStyleSheet("background-color: black;");
    }
    else if (data.contains(searchString3.toUtf8())) {
        ui->ledStatusDongle2->setStyleSheet("background-color: yellow;");
    }
    else if (data.contains(searchString4.toUtf8())) {
        ui->ledStatusDongle2->setStyleSheet("background-color: black;");
    }
    else if (data.contains(searchString5.toUtf8())) {
        ui->ledStatusDongle3->setStyleSheet("background-color: yellow;");
    }
    else if (data.contains(searchString6.toUtf8())) {
        ui->ledStatusDongle3->setStyleSheet("background-color: black;");
    }
    one = 1;
}
/**
 * @brief Connects to the selected COM port.
 */
void MainWindow::connectCom()
{
    int x = ui->ComSpinBox->value();

    // Convert int to QString and concatenate with the port name prefix
    QString portName = "COM" + QString::number(x);
    qDebug() << portName;
    serial->setPortName(portName);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        // Handle error
    }
}

/**
 * @brief Closes the connection to the COM port.
 */
void MainWindow::closeCom()
{
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "Closed";
    }
}

void MainWindow::dataDongle1()
{
    toggle1 = sendData(Dongle1, toggle1);
}

void MainWindow::dataDongle2()
{
    toggle2 = sendData(Dongle2, toggle2);
}

void MainWindow::dataDongle3()
{
    toggle3 = sendData(Dongle3, toggle3);
}

int MainWindow::sendData(QString Dongle, int toggle)
{
    QString Char;
    if (toggle == 0) {
        Char = "BTN\r";
            //"1\r" +Dongle;
        toggle = 1;
    } else {
        Char = "BTN\r";
            //"0\r" +Dongle;
        toggle = 0;
    }

    QByteArray byte = Char.toUtf8();
    serial->write(byte);
    qDebug() << Char;
    return toggle;
}

void MainWindow::makeRadioGroups()
{
    QList<QRadioButton *> allButtonsB = ui->groupBoxButton->findChildren<QRadioButton *>();
    QList<QRadioButton *> allButtonsL = ui->groupBoxLed->findChildren<QRadioButton *>();
    for(int i = 0; i < allButtonsB.size(); ++i)
    {
        groupB.addButton(allButtonsB[i],i);
        groupL.addButton(allButtonsL[i],i);
    }
}

void MainWindow::connectDongle()
{
    int B;
    int L;
    B = groupB.checkedId();
    L = groupL.checkedId();
    QString BS;
    QString LS;
    QString combinedString;
    switch (B) {
    case 0:
        BS = Dongle1;
        break;
    case 1:
        BS = Dongle2;
        break;
    case 2:
        BS = Dongle3;
        break;
    default:
        // Handle other cases if needed
        BS = "unknown";
        break;
    }

    switch (L) {
    case 0:
        LS = Dongle1;
        break;
    case 1:
        LS = Dongle2;
        break;
    case 2:
        LS = Dongle3;
        break;
    default:
        // Handle other cases if needed
        LS = "unknown";
        break;
    }

    combinedString ="B" + BS + "L" + LS;
    qDebug() << combinedString;
}
