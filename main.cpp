#include "widget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("andreag");
    QCoreApplication::setApplicationName("Volume");
    Widget w;
    w.show();
    return a.exec();
}
