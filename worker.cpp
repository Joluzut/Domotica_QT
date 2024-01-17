#include "worker.h"

Worker::Worker(QObject *parent)
    : QObject{parent}
{
    qInfo() << this << "Constructed"  << QThread::currentThread();
}

Worker::~Worker()
{
    qInfo() << this << "Deconstructed"  << QThread::currentThread();
}

void Worker::run(QSerialPort *serial)
{
    data = serial->readAll();
}

QByteArray Worker::giveData()
{
    return data;
}
