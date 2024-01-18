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
    connect(serial, &QSerialPort::readyRead, this,&MainWindow::startThread);
    connect(ui->ComButton, &QPushButton::clicked, this, &MainWindow::connectCom);
    connect(ui->ComDisButton, &QPushButton::clicked, this, &MainWindow::closeCom);
    connect(ui->ButtonDongle1, &QPushButton::clicked, this, &MainWindow::dataDongle1);
    connect(ui->ButtonDongle2, &QPushButton::clicked, this, &MainWindow::dataDongle2);
    connect(ui->ButtonDongle3, &QPushButton::clicked, this, &MainWindow::dataDongle3);
    connect(ui->ButtonCouple, &QPushButton::clicked, this, &MainWindow::connectDongle);
    connect(ui->ButtonRefresh, &QPushButton::clicked, this, &MainWindow::refreshStatus);
    connect(ui->ButtonReset, &QPushButton::clicked, this, &MainWindow::resetAll);
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
        qDebug() << "startThread called too soon after last call";
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

    for(int k = 0; k < 5; k++)
    {
        LocDongle[k] = "G0xC000";
    }
}

void MainWindow::connectDongle()
{
    int B;
    int L;
    int G;
    B = groupB.checkedId();
    L = groupL.checkedId();
    G = groupG.checkedId();
    QString BS;
    QString LS;
    QString GS;

    switch (G) {
    case 0:
        GS = "G0xC001";
        break;
    case 1:
        GS = "G0xC002";
        break;
    case 2:
        GS = "G0xC003";
        break;
    default:
        // Handle other cases if needed
        GS = "unknown";
        break;
    }

    switch (B) {
    case 0:
        BS = "B"+Dongle1;
        radioButtonData("RM"+BS+LocDongle[0]+"\r");
        qDebug() << "RM"+BS+LocDongle[0]+"\r";
        LocDongle[0] = GS;
        break;
    case 1:
        BS = "B"+Dongle2;
        radioButtonData("RM"+BS+LocDongle[2]+"\r");
        qDebug() << "RM"+BS+LocDongle[2]+"\r";
        LocDongle[2] = GS;
        break;
    case 2:
        BS = "B"+Dongle3;
        radioButtonData("RM"+BS+LocDongle[4]+"\r");
        qDebug() << "RM"+BS+LocDongle[4]+"\r";
        LocDongle[4] = GS;
        break;
    case 3:
        BS = "nothing";
        qDebug() << "boomboclad";
        break;
    default:
        // Handle other cases if needed
        BS = "unknown";
        break;
    }
    radioButtonData("ADD"+BS+GS+"\r");
    qDebug() << "ADD"+BS+GS+"\r";
    switch (L) {
    case 0:
        LS = "L"+Dongle1;
        radioButtonData("RM"+LS+LocDongle[1]+"\r");
        qDebug() << "RM"+LS+LocDongle[1]+"\r";
        LocDongle[1] = GS;
        break;
    case 1:
        LS = "L"+Dongle2;
        radioButtonData("RM"+LS+LocDongle[3]+"\r");
        qDebug() << "RM"+LS+LocDongle[3]+"\r";
        LocDongle[3] = GS;
        break;
    case 2:
        LS = "L"+Dongle3;
        radioButtonData("RM"+LS+LocDongle[5]+"\r");
        qDebug() << "RM"+LS+LocDongle[5]+"\r";
        LocDongle[5] = GS;
        break;
    case 3:
        LS = "nothing";
        break;
    default:
        // Handle other cases if needed
        LS = "unknown";
        break;
    }

    radioButtonData("ADD"+LS+GS+"\r");
    qDebug() << "ADD"+LS+GS+"\r";
}
void MainWindow::radioButtonData(QString string)
{
    QByteArray array = string.toUtf8();
    serial->write(array);
    QThread::msleep(1000);
}

void MainWindow::refreshStatus()
{
    serial->write("getLed0x0001\r");
    QThread::msleep(100);
    serial->write("getLed0x0002\r");
    QThread::msleep(100);
    serial->write("getLed0x0003\r");
    QThread::msleep(100);

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
    serial->write("RESET");
}
