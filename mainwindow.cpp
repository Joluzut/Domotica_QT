#include <QButtonGroup>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    serial(new QSerialPort(this))
{
    ui->setupUi(this);
    makeRadioGroups();

    //add addresses to the dongles
    dongle1.address = "0x0001";
    dongle2.address = "0x0002";
    dongle3.address = "0x0003";


    // Connect signals and slots
    //When a text comes in by the serial port a thread is started.
    connect(serial, &QSerialPort::readyRead, this,&MainWindow::startThread);
    connect(ui->ComButton, &QPushButton::clicked, this, &MainWindow::connectCom);
    connect(ui->ComDisButton, &QPushButton::clicked, this, &MainWindow::closeCom);
    connect(ui->ButtonDongle1, &QPushButton::clicked, this, &MainWindow::dataDongle1);
    connect(ui->ButtonDongle2, &QPushButton::clicked, this, &MainWindow::dataDongle2);
    connect(ui->ButtonDongle3, &QPushButton::clicked, this, &MainWindow::dataDongle3);
    connect(ui->ButtonCouple, &QPushButton::clicked, this, &MainWindow::connectDongle);
    connect(ui->ButtonRefresh, &QPushButton::clicked, this, &MainWindow::refreshStatus);
    connect(ui->ButtonReset, &QPushButton::clicked, this, &MainWindow::resetAll);
    connect(ui->ButtonGetAllInfo, &QPushButton::clicked, this, &MainWindow::getInfo);
}

MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::startThread()
{
    static QElapsedTimer timer;
    if (timer.isValid() && timer.elapsed() < 800)
    {
        return;
    }

    timer.start();
    qDebug() << "startThread called";
    serialworker = new Worker([this]() {
        readSerialThread();
    });
    connect(serialworker, &Worker::finished, this, &MainWindow::readSerialThreadFinished);
    // Start the worker
    QThreadPool::globalInstance()->start(serialworker);
}

void MainWindow::readSerialThread()
{
    // Wait for 1 second
    QThread::msleep(500);
    qDebug() << "THREAD " << QThread::currentThreadId();
}

void MainWindow::readSerialThreadFinished()
{
    handleReadyRead();
    serialworker = nullptr;
    qDebug() << "FINISHED";
}

void MainWindow::handleReadyRead()
{
    //read all data recieved and sends them into the data variable
    data = serial->readAll();
    ui->textBrowser->append(data);
    //This is what the dongles sends back when a get request is recieved
    QString searchString1 = "LEDSTAT0A0x0001";
    QString searchString2 = "LEDSTAT1A0x0001";
    QString searchString3 = "LEDSTAT0A0x0002";
    QString searchString4 = "LEDSTAT1A0x0002";
    QString searchString5 = "LEDSTAT0A0x0003";
    QString searchString6 = "LEDSTAT1A0x0003";

    //These function check if the recieved data compares the gives strings
    if (data.contains(searchString1.toUtf8())) {
        ui->ledStatusDongle1->setStyleSheet("background-color: black;");
    }
    if (data.contains(searchString2.toUtf8())) {
        ui->ledStatusDongle1->setStyleSheet("background-color: yellow;");
    }
    if (data.contains(searchString3.toUtf8())) {
        ui->ledStatusDongle2->setStyleSheet("background-color: black;");
    }
    if (data.contains(searchString4.toUtf8())) {
        ui->ledStatusDongle2->setStyleSheet("background-color: yellow;");
    }
    if (data.contains(searchString5.toUtf8())) {
        ui->ledStatusDongle3->setStyleSheet("background-color: black;");
    }
    if (data.contains(searchString6.toUtf8())) {
        ui->ledStatusDongle3->setStyleSheet("background-color: yellow;");
    }
}

void MainWindow::connectCom()
{
    int x = ui->ComSpinBox->value();

    // Convert int to QString and concatenate with the port name prefix
    QString portName = "COM" + QString::number(x);
    qDebug() << portName;
    // portName = "/dev/tty.usbmodem0006851197611";
    serial->setPortName(portName);

    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!serial->open(QIODevice::ReadWrite)) {
        // Handle error
    }
}


void MainWindow::closeCom()
{
    //close the COM port
    if (serial->isOpen()) {
        serial->close();
        qDebug() << "Closed";
    }
}

void MainWindow::dataDongle1()
{
    //send BUTTON to the DK
    serial->write("BUTTON\r");
}

void MainWindow::dataDongle2()
{
    //send request to dongle 2
    toggle2 = sendData(dongle2.address, toggle2);
}

void MainWindow::dataDongle3()
{
    //send request to dongle 3
    toggle3 = sendData(dongle3.address, toggle3);
}

int MainWindow::sendData(QString Dongle, int toggle)
{
    //toggle for sending dongles on and off
    QString Char;
    if (toggle == 0) {
        Char = "S1L" +Dongle+"\r";
        toggle = 1;
    } else {
        Char = "S0L" +Dongle+"\r";
        toggle = 0;
    }

    QByteArray byte = Char.toUtf8();
    serial->write(byte);
    qDebug() << Char;
    return toggle;
}

void MainWindow::makeRadioGroups()
{
    //radio buttons need to be added to a group for easier use with this function all buttons will be added to groups.
    QList<QRadioButton *> allButtonsB = ui->groupBoxButton->findChildren<QRadioButton *>();
    QList<QRadioButton *> allButtonsL = ui->groupBoxLed->findChildren<QRadioButton *>();
    QList<QRadioButton *> allButtonsG = ui->groupBoxGroups->findChildren<QRadioButton *>();
    for(int i = 0; i < allButtonsB.size(); ++i)
    {
        groupB.addButton(allButtonsB[i],i);
        groupL.addButton(allButtonsL[i],i);
    }
    for(int j = 0; j < allButtonsG.size(); ++j)
    {
        groupG.addButton(allButtonsG[j],j);
    }

    //all leds and buttons are standard set to group 0xC000 so to mirror the server and clients are also set to this address
    dongle1.server = "0xC000";
    dongle2.server = "0xC000";
    dongle3.server = "0xC000";
    dongle1.client = "0xC000";
    dongle2.client = "0xC000";
    dongle3.client = "0xC000";

}

void MainWindow::startConnectThread()
{
    static QElapsedTimer timer;
    if (timer.isValid() && timer.elapsed() < 800)
    {
        // qDebug() << "startThread called too soon after last call";
        return;
    }

    timer.start();
    qDebug() << "startThread called";
    connectWorker = new Worker([this]() {
        connectDongle();
    });
    connect(connectWorker, &Worker::finished, this, &MainWindow::connectThreadFinished);
    // Start the worker
    QThreadPool::globalInstance()->start(connectWorker);
}

void MainWindow::connectThreadFinished()
{

}

void MainWindow::connectDongle()
{
    //The couple function for leds buttons and groups.
    int B = groupB.checkedId();
    int L = groupL.checkedId();
    int G = groupG.checkedId();
    qDebug() << "B: " << B << " L: " << L << " G: " << G;
    QString BUTTONADDR;
    QString LEDADDR;
    QString GROUPADDR;
    QString RMB;
    QString RML;
    bool changeButton = true;
    bool changeLed = true;
    QString* updateLedADDR;
    QString* updateButtonADDR;

    //switch case for buttons
    switch(B)
    {
    case 0:
        BUTTONADDR = dongle1.address;//dongle address that has been pressed
        updateButtonADDR = &dongle1.client;//button address that has been pressed
        break;
    case 1:
        BUTTONADDR = dongle2.address;
        updateButtonADDR = &dongle2.client;
        break;
    case 2: 
        BUTTONADDR = dongle3.address;
        updateButtonADDR = &dongle3.client;
        break;
    case 3:
        changeButton = false;
        break;
    }

    //switch case for leds
    switch(L)
    {
    case 0:
        LEDADDR = dongle1.address;//dongle address that has been pressed
        updateLedADDR = &dongle1.server;//led address that has been pressed
        break;
    case 1:
        LEDADDR = dongle2.address;
        updateLedADDR = &dongle2.server;
        break;
    case 2: 
        LEDADDR = dongle3.address;
        updateLedADDR = &dongle3.server;
        break;   
    case 3:
        LEDADDR = "";
        changeLed = false;
        break;
    }

    //switch case for groups
    switch(G)
    {
    case 0:
        GROUPADDR = "0xC001";//group address that has been pressed
        break;
    case 1:
        GROUPADDR = "0xC002";
        break;
    case 2:
        GROUPADDR = "0xC003";
        break;
    }

    if(changeButton)
    {
        //if the button has been changed send this to the dongles
        RMB = "CB" + BUTTONADDR + "G" + GROUPADDR + "GO" + *updateButtonADDR + "\r";
        radioButtonData(RMB);
        qDebug() << RMB;
        *updateButtonADDR = GROUPADDR;
    }

    if(changeLed)
    {
        //if the led has been changed send this to the dongles
        RML = "CL" + LEDADDR + "G" + GROUPADDR + "GO" + *updateLedADDR + "\r";
        radioButtonData(RML);
        qDebug() << RML;
        *updateLedADDR = GROUPADDR;
    }

}
void MainWindow::radioButtonData(QString string)
{
    //send data to the dongle after changing str to uft8
    QByteArray array = string.toUtf8();
    serial->write(array);
}

void MainWindow::refreshStatus()
{
    //when this is send all dongles will send back there status
    serial->write("getLed0x0001\r");
    serial->write("getLed0x0002\r");
    serial->write("getLed0x0003\r");
}

void MainWindow::resetAll()
{
    //when this is send all dongles will reset to no group
    serial->write("RESET\r");
}

void MainWindow::getInfo()
{

    radioButtonData("ALL0x0001\r");
    radioButtonData("ALL0x0002\r");
    radioButtonData("ALL0x0003\r");

}
