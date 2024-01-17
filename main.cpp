#include "mainwindow.h"
#include <QThread>
#include <QDebug>
#include "worker.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;


    //QThread thread;
    //thread.setObjectName("Worker Thread");
    //worker->moveToThread(&thread);
    //QObject::connect(&thread,&QThread::started,worker,&Worker::run);


    qInfo() << "Finished"  << QThread::currentThread();

    w.show();

    return a.exec();
}
