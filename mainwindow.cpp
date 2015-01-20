/*
 * marching_cubes.cpp
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 */

#include "mainwindow.h"
#include "marching_cubes.h"

#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QMainWindow *parent, GLuint Resolution, Qt::WindowFlags flags): QMainWindow(parent,flags)
{
    //gl_widget gives access to GL_Widget class
    gl_widget       = new GL_Widget(0,Resolution);  //Child on Heap
    statustextlabel = new QLabel();                 //Child on Heap

    setCentralWidget(gl_widget);

    //Menu Items
        QMenu *lev_Menu = new QMenu(tr("&Level Sets      |"),this);
        QMenu *pat_Menu = new QMenu(tr("&Partition  Mode |"),this);
        QMenu *pol_Menu = new QMenu(tr("&Polygon    Mode |"),this);
        QMenu *pro_Menu = new QMenu(tr("&Projection Mode |"),this);
        QMenu *bld_Menu = new QMenu(tr("&Blending   Mode"),this);

        menuBar()->addMenu(lev_Menu);
        menuBar()->addMenu(pat_Menu);
        menuBar()->addMenu(pol_Menu);
        menuBar()->addMenu(pro_Menu);
        menuBar()->addMenu(bld_Menu);

    //Menu Actions
        lev_Menu->addAction(tr("Potential Function   "),this,SLOT(PT1()));
        lev_Menu->addAction(tr("Toroidal Surface     "),this,SLOT(PT2()));
        lev_Menu->addAction(tr("Double Torus         "),this,SLOT(PT3()));
        lev_Menu->addAction(tr("Chmutov Surface 1    "),this,SLOT(PT4()));
        lev_Menu->addAction(tr("Chmutov Surface 2    "),this,SLOT(PT5()));
        lev_Menu->addAction(tr("Klein Bottle         "),this,SLOT(PT6()));
        lev_Menu->addAction(tr("Hydrogen Superpos.State "),this,SLOT(PT7()));
        lev_Menu->addAction(tr("Minkowski Space      "),this,SLOT(PT8()));

        pat_Menu->addAction(tr("Cubes       "),this,SLOT(PR1()));
        pat_Menu->addAction(tr("Tetrahedrons"),this,SLOT(PR2()));

        pol_Menu->addAction(tr("Filled Triangles "),this,SLOT(PG1()));
        pol_Menu->addAction(tr("Wireframe        "),this,SLOT(PG2()));

        pro_Menu->addAction(tr("Orthographic "),this,SLOT(PJ1()));
        pro_Menu->addAction(tr("Perspective  "),this,SLOT(PJ2()));

        bld_Menu->addAction(tr("On "),this,SLOT(BL1()));
        bld_Menu->addAction(tr("Off"),this,SLOT(BL2()));

    //Status Bar
        statusBar();
        update_statusBar();
}

MainWindow::~MainWindow() { delete gl_widget; delete statustextlabel;}

//Level Set Actions
    void MainWindow::PT1(void)		{gl_widget->Level_Set_Type(1);}
    void MainWindow::PT2(void)		{gl_widget->Level_Set_Type(2);}
    void MainWindow::PT3(void)		{gl_widget->Level_Set_Type(3);}
    void MainWindow::PT4(void)		{gl_widget->Level_Set_Type(4);}
    void MainWindow::PT5(void)		{gl_widget->Level_Set_Type(5);}
    void MainWindow::PT6(void)		{gl_widget->Level_Set_Type(6);}
    void MainWindow::PT7(void)		{gl_widget->Level_Set_Type(7);}
    void MainWindow::PT8(void)		{gl_widget->Level_Set_Type(8);}

//Partition Mode Actions
    void MainWindow::PR1(void)		{gl_widget->Partition_Mode(1);}
    void MainWindow::PR2(void)		{gl_widget->Partition_Mode(2);}

//Polygon Mode Actions
    void MainWindow::PG1(void)		{gl_widget->Polygon_Mode(1);}
    void MainWindow::PG2(void)		{gl_widget->Polygon_Mode(2);}

//Projection Mode Actions
    void MainWindow::PJ1(void)		{gl_widget->Projection_Mode(1);}
    void MainWindow::PJ2(void)		{gl_widget->Projection_Mode(2);}

//Blending Mode Actions
    void MainWindow::BL1(void)		{gl_widget->Blending_Mode(1);}
    void MainWindow::BL2(void)		{gl_widget->Blending_Mode(2);}

void MainWindow::update_statusBar()
{
        statustextlabel->setText("Level Sets scaned by MC Algorithm| OpenGL Support | Binary GL Driver");
        statustextlabel->setAlignment(Qt::AlignHCenter);
        statustextlabel->setFrameStyle(QFrame::Panel | QFrame::Raised);
        statustextlabel->setLineWidth(3);
        statusBar()->addWidget(statustextlabel,1);
}

