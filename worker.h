#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    QByteArray giveData();

    ~Worker();

signals:

public slots:
    void run(QSerialPort *serial);

private:
    QByteArray data;
};

#endif // WORKER_H
