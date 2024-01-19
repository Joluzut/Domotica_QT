#include "worker.h"

Worker::Worker(std::function<void()> func) : func(func) {}
Worker::~Worker()
{

}
void Worker::run() {
    isRunning_ = true;
    func();
    emit finished(); // Emit the signal when the work is done
    isRunning_ = false;
}
