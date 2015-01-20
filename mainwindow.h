/*
 * marching_cubes.cpp
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 */

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "marching_cubes.h"


/* -------------- Main Window -------------- */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QMainWindow *parent = 0, GLuint Resolution = 100, Qt::WindowFlags flags = 0);
    ~MainWindow();

private:
    void update_statusBar(void);

    GL_Widget   *gl_widget;         //points to Central Widget
    QLabel      *statustextlabel;   //points to Status Message Label

private slots:
    void PT1(void);
    void PT2(void);
    void PT3(void);
    void PT4(void);
    void PT5(void);
    void PT6(void);
    void PT7(void);
    void PT8(void);

    void PR1(void);
    void PR2(void);

    void PG1(void);
    void PG2(void);

    void PJ1(void);
    void PJ2(void);

    void BL1(void);
    void BL2(void);

};

#endif /* MAINWINDOW_H_ */
