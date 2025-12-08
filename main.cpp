/******************************************************************
 * main.cpp
 *
 * This file contains the entry point for the Qt application.
 * It creates a QApplication object, constructs the MainWindow,
 * shows it on the screen, and then starts the event loop.
 *
 ******************************************************************/

#include "mainwindow.h"
#include <QApplication>

/******************************************************************
 * main --
 *   Program entry point. Initializes the Qt application,
 *   creates the main window, and starts the event loop.
 *
 * Parameters:
 *   argc - number of command-line arguments
 *   argv - array of C-strings containing the arguments
 *
 * Returns:
 *   int - exit code from QApplication::exec()
 ******************************************************************/
int main(int argc, char *argv[])
{
    // Create the Qt application object (handles GUI + event loop)
    QApplication a(argc, argv);

    // Create and show the main window for the cafeteria system
    MainWindow w;
    w.show();

    // Enter the Qt event loop; program ends when the window closes
    return a.exec();
}
