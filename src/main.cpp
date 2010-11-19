#include <QtGui/QApplication>
#include <QtCore/QSettings>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow * mw = new MainWindow(0);
    mw->show();
    int retval = app.exec();
    delete mw;
    return retval;
}
