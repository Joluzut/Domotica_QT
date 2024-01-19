#ifndef WORKER_H
#define WORKER_H

#include <QRunnable>
#include <functional>
#include <QObject>

class Worker : public QObject, public QRunnable {
    Q_OBJECT // Add Q_OBJECT macro for signals/slots support

public:
    Worker(std::function<void()> func);
    ~Worker();
    void run() override;
    bool isRunning() const { return isRunning_; }

signals:
    void finished(); // Custom signal to indicate work is done

private:
    std::function<void()> func;
    bool isRunning_ = false;
};

#endif // WORKER_H
