/** Projekt: Warp
*   Autor:   Miroslav Schery
*            Zsolt Horv�th
*
*   Posledna uprava: 6.5.2011
*/

#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
