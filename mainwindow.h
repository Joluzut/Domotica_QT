#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QButtonGroup>
#include <QThread>
#include <QDebug>
#include <QThreadPool>
#include "worker.h"

/**
 * @brief The MainWindow class
 *
 * The MainWindow class represents the main window of the application.
 */

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow
     * @param parent The parent widget (default is nullptr)
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor for MainWindow
     */
    ~MainWindow();

    /**
     * @brief Slot function to handle data ready to be read from the serial port
     */
    void handleReadyRead();

    /**
     * @brief Connect to the serial port
     */
    void connectCom();

    /**
     * @brief Close the connection to the serial port
     */
    void closeCom();

    /**
     * @brief Send data through the serial port
     */
    void dataDongle1();
    void dataDongle2();
    void dataDongle3();
    void makeRadioGroups();
    void connectDongle();
    int sendData(QString, int);
    void startThread();
    void readSerialThread();
    void readSerialThreadFinished();
    void refreshStatus();
    void radioButtonData(QString);
    void resetAll();


private:
    Ui::MainWindow *ui;  ///< The user interface object
    QSerialPort *serial;  ///< The serial port object
    int toggle1 = 0;       ///< Toggle variable
    int toggle2 = 0;
    int toggle3 = 0;
    int one = 0;          ///< Another variable
    QButtonGroup groupB;
    QButtonGroup groupL;
    QButtonGroup groupG;
    QString Dongle1 = "0x0001";
    QString Dongle2 = "0x0002";
    QString Dongle3 = "0x0003";
    QByteArray data; //Data from serial
    Worker* serialworker;
    QString LocDongle[6];
};
#endif // MAINWINDOW_H
