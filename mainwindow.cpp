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

    dongle1.address = "0x0001";
    dongle2.address = "0x0002";
    dongle3.address = "0x0003";


    // Connect signals and slots
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

/**
 * @brief Destructor for the MainWindow class.
 */
MainWindow::~MainWindow()
{
    delete ui;

}

void MainWindow::startThread()
{
    static QElapsedTimer timer;
    if (timer.isValid() && timer.elapsed() < 800)
    {
        // qDebug() << "startThread called too soon after last call";
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

    // data = worker->giveData();
    data = serial->readAll();
    // ui->textBrowser->clear();
    ui->textBrowser->append(data);
    ui->textBrowser1->append(data);
    QString searchString1 = "LEDSTAT0A0x0001";
    QString searchString2 = "LEDSTAT1A0x0001";
    QString searchString3 = "LEDSTAT0A0x0002";
    QString searchString4 = "LEDSTAT1A0x0002";
    QString searchString5 = "LEDSTAT0A0x0003";
    QString searchString6 = "LEDSTAT1A0x0003";
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
/**
 * @brief Connects to the selected COM port.
 */
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
    serial->write("BUTTON\r");
}

void MainWindow::dataDongle2()
{
    toggle2 = sendData(dongle2.address, toggle2);
}

void MainWindow::dataDongle3()
{
    toggle3 = sendData(dongle3.address, toggle3);
}

int MainWindow::sendData(QString Dongle, int toggle)
{
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
    int B = groupB.checkedId();
    int L = groupL.checkedId();
    int G = groupG.checkedId();
    qDebug() << "B: " << B << " L: " << L << " G: " << G;
    QString BUTTONADDR;
    // QString RMBUTTONADDR;
    // QString RMLEDADDR;
    QString LEDADDR;
    QString GROUPADDR;
    QString RMB;
    QString ADDB;
    QString RML;
    QString ADDL;
    bool changeButton = true;
    bool changeLed = true;
    QString* updateLedADDR;
    QString* updateButtonADDR;

    switch(B)
    {
    case 0:
        BUTTONADDR = dongle1.address;
        // RMBUTTONADDR = dongle1.client;
        updateButtonADDR = &dongle1.client;
        break;
    case 1:
        BUTTONADDR = dongle2.address;
        // RMBUTTONADDR = dongle2.client;
        updateButtonADDR = &dongle2.client;
        break;
    case 2: 
        BUTTONADDR = dongle3.address;
        // RMBUTTONADDR = dongle3.client;
        updateButtonADDR = &dongle3.client;
        break;
    case 3:
        changeButton = false;
        break;
    }


    switch(L)
    {
    case 0:
        LEDADDR = dongle1.address;
        // RMLEDADDR = dongle1.server;
        updateLedADDR = &dongle1.server;
        break;
    case 1:
        LEDADDR = dongle2.address;
        // RMLEDADDR = dongle2.server;
        updateLedADDR = &dongle2.server;
        break;
    case 2: 
        LEDADDR = dongle3.address;
        // RMLEDADDR = dongle3.server;
        updateLedADDR = &dongle3.server;
        break;   
    case 3:
        LEDADDR = "";
        changeLed = false;
        break;
    }

    switch(G)
    {
    case 0:
        GROUPADDR = "0xC001";
        break;
    case 1:
        GROUPADDR = "0xC002";
        break;
    case 2:
        GROUPADDR = "0xC003";
        break;
    }
    // serial->write("TEST\r");
    // QThread::msleep(100);
    if(changeButton)
    {
        // RMB = "RMB" + BUTTONADDR + "G" + *updateButtonADDR + "\r";
        RMB = "CB" + BUTTONADDR + "G" + GROUPADDR + "GO" + *updateButtonADDR + "\r";
        radioButtonData(RMB);
        qDebug() << RMB;
        // ADDB = "ADDB" + BUTTONADDR + "G" + GROUPADDR + "\r";
        // radioButtonData("ADDB" + BUTTONADDR + "G" + GROUPADDR + "\r");
        // qDebug() << ADDB;
        *updateButtonADDR = GROUPADDR;
    }
    // QThread::msleep(100);
    // serial->write("TEST\r");
    // QThread::msleep(2000);
    if(changeLed)
    {
        // RML = "RML" + LEDADDR + "G" + *updateLedADDR + "\r";
        RML = "CL" + LEDADDR + "G" + GROUPADDR + "GO" + *updateLedADDR + "\r";
        radioButtonData(RML);
        qDebug() << RML;
        // ADDL = "ADDL" + LEDADDR + "G" + GROUPADDR + "\r";
        // radioButtonData("ADDL" + LEDADDR + "G" + GROUPADDR + "\r");
        // qDebug() << ADDL;
        *updateLedADDR = GROUPADDR;
    }

}
void MainWindow::radioButtonData(QString string)
{
    QByteArray array = string.toUtf8();
    // qDebug() << "RadiobuttonDataString: " << string;
    serial->write(array);
    // QThread::msleep(300);
}

void MainWindow::refreshStatus()
{
    serial->write("getLed0x0001\r");
    // QThread::msleep(100);
    serial->write("getLed0x0002\r");
    // QThread::msleep(100);
    serial->write("getLed0x0003\r");
    // QThread::msleep(100);

}

void MainWindow::resetAll()
{
 /*   for (int G = 0; G <= 3; G++)
    {
        for (int L = 1; L <= 3; L++)
        {
            radioButtonData("RML0x000" + QString::number(L) + "G0xC00" + QString::number(G) + "\r");
            qDebug() << "RML0x000" + QString::number(L) + "G0xC00" + QString::number(G) + "\r";
            radioButtonData("RMB0x000" + QString::number(L) + "G0xC00" + QString::number(G) + "\r");
            qDebug() << "RMB0x000" + QString::number(L) + "G0xC00" + QString::number(G) + "\r";
        }
    }*/
    serial->write("RESET\r");
}

void MainWindow::getInfo()
{

    radioButtonData("ALL0x0001\r");
    radioButtonData("ALL0x0002\r");
    radioButtonData("ALL0x0003\r");

}
