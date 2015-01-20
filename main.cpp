/*
 * marching_cubes.cpp
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 */

#include "mainwindow.h"

#include <QtGui>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setFont(QFont("Helvetica",10,QFont::Bold));

    if(!QGLFormat::hasOpenGL())
    { cerr << "System has no OpenGL support" << endl; return 1; }

    /*
        - Path !                : Boost-Headers (Windows Platform & MinGW could cause trouble!)
        - Resolution            : MC-Algorithm Refinement
        - setGeometry(x,y,w,h)  : GL-Widget Position/Size
        - Generating Display Lists takes time !
    */

    MainWindow  mainwindow(0,/* Resolution = */ 120);    //on Stack
                mainwindow.setWindowTitle("Marching Cubes Algorithm , Scalar Fields & Level Sets");
                mainwindow.setGeometry(20,20,2520,1520);
                mainwindow.show();

    return app.exec();
}
