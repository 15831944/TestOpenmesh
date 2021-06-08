#include "mainwindow.h"

#include <QApplication>

#include <GCode/NCAction.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Initial init;

    a.setFont(QFont( "Microsoft YaHei UI", 12));

    MainWindow w;
    w.show();
//    float matrix[4][5] =
//    {
//        {0, 0, 0, 0, 0},
//        {0, 2, 3, 1, 6},
//        {0, 1, -1, 2, -1},
//        {0, 1, 2, -1, 5},
//    };
//    qDebug() << calc<float>(matrix);

    return a.exec();
}
