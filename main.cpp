#include "mainwindow.h"

#include <QApplication>

/**
 * @brief The main function of the application.
 *
 * This function initializes the QApplication, creates an instance of MainWindow,
 * shows the main window, and starts the application event loop.
 *
 * @param argc Number of command line arguments
 * @param argv Array of command line arguments
 * @return Application exit code
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Create an instance of the MainWindow
    MainWindow w;

    // Show the main window
    w.show();

    // Start the application event loop
    return a.exec();
}
