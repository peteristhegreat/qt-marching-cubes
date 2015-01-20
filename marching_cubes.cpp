/*
 * marching_cubes.cpp
 *  MC Algorithm implemented by Paul Bourke wiki contribution
 *  Qt-Adaption Created on: 15.07.2009
 *      Author: manitoo
 */

#include "marching_cubes.h"

GL_Widget::GL_Widget(QWidget *parent, GLuint resolution) : QGLWidget(parent)
{
    //Rendering Context
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));

    //Initialize Control Variables
    LEVEL_SET_TYPE      = 1;
    PARTITION_MODE      = 1;
    POLYGON_MODE        = 1;
    PROJECTION_MODE     = 1;
    BLENDING_MODE       = 1;

    //Computational Variables
    iDataSetSize    = resolution;
    fStepSize       = 1.0/iDataSetSize;

    //Wheel Scaling
    currentScaling  = 2.0;
    ZoomFactor      = 0.7;

    //Mouse Move Rotations
    rotX			= 0;
    rotY			= 0;
    rotZ			= 0;

    //Timer Animation
    timerId			= 0;
    t 				= 0.0;

    vec.append("Electrostatic Dodecahedron Potential");
    vec.append("Toroidal Surface (phantasy!)");
    vec.append("Double Torus  +x^8 -2 x^6 +x^4 +2(x^4 y^2) -2(x^2 y^2) +y^4 +z^2 = c");
    vec.append("Chmutov Surface 1");
    vec.append("Chmutov Surface 2");
    vec.append("Klein Bottle  (selfintersecting R^3 embedding)");
    vec.append("Unnormalized Hydrogen Superposition State |psi(r,theta,phi)(n=4 l=3 m=0)+psi(r,theta,phi)(n=5 l=4 m=0)|^2");
    vec.append("Minkowski Space  (isotrope lightcone, time-/spacelike subsets)");
}

GL_Widget::~GL_Widget() { }

/* --- Reimplemented virtual QGLWidget-Function --- */
void GL_Widget::initializeGL()
{
  qglClearColor(Qt::black);
        /* ------ Lights ------ */
        //Light Point Source
        GLfloat light1_position[]   = {+1.0,+2.0,+5.0, 1.0};
        GLfloat light1_ambient[]    = { 0.8, 0.8, 0.0, 0.1};
        GLfloat light1_diffuse[]    = { 0.2, 0.4, 1.0, 0.1};
        GLfloat light1_specular[]   = {+1.0,+1.0,+1.0, 0.1};

        glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
        glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
        glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);

        //Material
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 120.0);

        //Shading
        glShadeModel(GL_SMOOTH);

        //Blending  Enabled/Disabled in virtual paintGL()
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

        //Enable Lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT1);

        glEnable(GL_COLOR_MATERIAL);

        glEnable(GL_NORMALIZE);
        glEnable(GL_DEPTH_TEST);

        cout << "OpenGL Infos" << endl;
        cout << "GL_VENDOR      : " << glGetString(GL_VENDOR) << endl;
        cout << "GL_RENDERER    : " << glGetString(GL_RENDERER) << endl;
        cout << "GL_VERSION     : " << glGetString(GL_VERSION) << endl;
        //cout << "GL_EXTENSION   : " << glGetString(GL_EXTENSIONS) << endl;

        cout << "\n Generating GL Display Lists" << endl;
        cout << " (MC-Resolution) Data Set Size: " << iDataSetSize << endl;


    /* ------ Generate Display Lists ------ */

    //Potential Function
    Dodecahedron = glGenLists(1);
    glNewList(Dodecahedron,GL_COMPILE);
    glPointSize(4.0);
    glColor4f(0.5,0.5,0.5,1.0);
    //Vertices
    glBegin(GL_POINTS);
        for(size_t i = 0; i < sizeof(D)/sizeof(D[0]); i++)
        {            
            glVertex3f(D[i][0]+0.5,D[i][1]+0.5,D[i][2]+0.5);
        }
    glEnd();
    //Edges
    glBegin(GL_LINE_LOOP);
        for(size_t i = 0; i < sizeof(faces)/sizeof(faces[0]); i++)
        {
            glVertex3f(D[faces[i][0]][0]+0.5, D[faces[i][0]][1]+0.5, D[faces[i][0]][2]+0.5);
            glVertex3f(D[faces[i][1]][0]+0.5, D[faces[i][1]][1]+0.5, D[faces[i][1]][2]+0.5);
            glVertex3f(D[faces[i][2]][0]+0.5, D[faces[i][2]][1]+0.5, D[faces[i][2]][2]+0.5);
            glVertex3f(D[faces[i][3]][0]+0.5, D[faces[i][3]][1]+0.5, D[faces[i][3]][2]+0.5);
            glVertex3f(D[faces[i][4]][0]+0.5, D[faces[i][4]][1]+0.5, D[faces[i][4]][2]+0.5);
        }
    glEnd();

    glEndList();

    Level_Set_11 = glGenLists(1);
    glNewList(Level_Set_11,GL_COMPILE);
       glBegin(GL_TRIANGLES);
                vMarchingCubes(1, 1);
        glEnd();
    glEndList();

    Level_Set_12 = glGenLists(1);
    glNewList(Level_Set_12,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(1, 2);
        glEnd();
    glEndList();
    //Common Torus
    Level_Set_21 = glGenLists(1);
    glNewList(Level_Set_21,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(2, 1);
        glEnd();
    glEndList();

    Level_Set_22 = glGenLists(1);
    glNewList(Level_Set_22,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(2, 2);
        glEnd();
    glEndList();
    ///Double Torus
    Level_Set_31 = glGenLists(1);
    glNewList(Level_Set_31,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(3, 1);
        glEnd();
    glEndList();

    Level_Set_32 = glGenLists(1);
    glNewList(Level_Set_32,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(3, 2);
        glEnd();
    glEndList();
    //Chmutov Surface 1
    Level_Set_41 = glGenLists(1);
    glNewList(Level_Set_41,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(4, 1);
        glEnd();
    glEndList();

    Level_Set_42 = glGenLists(1);
    glNewList(Level_Set_42,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(4, 2);
        glEnd();
    glEndList();
    //Chmutov Surface 2
    Level_Set_51 = glGenLists(1);
    glNewList(Level_Set_51,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(5, 1);
        glEnd();
    glEndList();

    Level_Set_52 = glGenLists(1);
    glNewList(Level_Set_52,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(5, 2);
        glEnd();
    glEndList();
    //Klein Bottle
    Level_Set_61 = glGenLists(1);
    glNewList(Level_Set_61,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(6, 1);
        glEnd();
    glEndList();

    Level_Set_62 = glGenLists(1);
    glNewList(Level_Set_62,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(6, 2);
        glEnd();
    glEndList();
    //Hydrogen Eigenfunction
    Level_Set_71 = glGenLists(1);
    glNewList(Level_Set_71,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(7, 1);
        glEnd();
    glEndList();

    Level_Set_72 = glGenLists(1);
    glNewList(Level_Set_72,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(7, 2);
        glEnd();
    glEndList();
    //Minkowski Space
    Level_Set_81 = glGenLists(1);
    glNewList(Level_Set_81,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(8, 1);
        glEnd();
    glEndList();

    Level_Set_82 = glGenLists(1);
    glNewList(Level_Set_82,GL_COMPILE);
        glBegin(GL_TRIANGLES);
                vMarchingCubes(8, 2);
        glEnd();
    glEndList();
}

void GL_Widget::resizeGL(GLsizei w, GLsizei h)
{
  switch ( PROJECTION_MODE )
  { case 1 : //Orthographic Projection
   {
        GLfloat nRange = 1.2;
        if (h == 0)	h = 1;
        glViewport(0,0,w,h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        if ( w <= h )
        glOrtho(-nRange,+nRange,-nRange*h/w,+nRange*h/w,-nRange,+nRange);
        else
        glOrtho(-nRange*w/h,+nRange*w/h,-nRange,+nRange,-nRange,+nRange);

        glMatrixMode(GL_MODELVIEW);
        glColor3f(1.0,0.8,0.0); renderText(int(0.01*width()),int(0.035*height()),"Orthographic Projection"); break;
   }
   case 2 : //Perspective Projection
      {
        if(h == 0)	h = 1;
        glViewport(0,0,w,h);
        GLfloat fAspect = (GLfloat)w/(GLfloat)h;
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(110.0, fAspect, 0.25, +800.0);
        glTranslatef(0.0,0.0,-1.4);
        glMatrixMode(GL_MODELVIEW);
        glColor3f(0.0,0.8,1.0); renderText(int(0.01*width()),int(0.035*height()),"Perspective Projection"); break;
     }
  }
}

/* --- Reimplemented virtual QGLWidget-Function --- */
void GL_Widget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    resizeGL(width(),height());
    glLoadIdentity();

    //Mouse Move Rotations
    glRotatef(rotX,1.0,0.0,0.0);
    glRotatef(rotY,0.0,1.0,0.0);
    glRotatef(rotZ,0.0,0.0,1.0);

    axes(1.25, 1.1, 1.25, 0.0, 1.0, 0.0);

    //Wheel Scaling
    glScalef(currentScaling,currentScaling,currentScaling);

    //Timer Animation
    GLfloat timerScaling = 0.5+pow(cos(0.025*t),2);
    glScalef(timerScaling,timerScaling,timerScaling);
    glRotatef(t,1,1,1);

    axes(0.7, 0.7, 0.7, 1.0, 1.0, 0.0);

    glTranslatef(-0.5, -0.5, -0.5);

  //Select Polygon Mode
  switch ( POLYGON_MODE )
  {	case 1: { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break; }
        case 2: { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break; }
        default:	break;
  }

  //Select Blending Mode
  switch ( BLENDING_MODE )
  {	case 1: { glEnable(GL_BLEND); break; }
        case 2: { glDisable(GL_BLEND); break; }
        default:	break;
  }


    //Select & Show Display Lists
    if (LEVEL_SET_TYPE == 1 && PARTITION_MODE == 1) { glCallList(Dodecahedron); glCallList(Level_Set_11); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[0]);}
    if (LEVEL_SET_TYPE == 2 && PARTITION_MODE == 1) { glCallList(Level_Set_21); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[1]);}
    if (LEVEL_SET_TYPE == 3 && PARTITION_MODE == 1) { glCallList(Level_Set_31); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[2]);}
    if (LEVEL_SET_TYPE == 4 && PARTITION_MODE == 1) { glCallList(Level_Set_41); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[3]);}
    if (LEVEL_SET_TYPE == 5 && PARTITION_MODE == 1) { glCallList(Level_Set_51); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[4]);}
    if (LEVEL_SET_TYPE == 6 && PARTITION_MODE == 1) { glCallList(Level_Set_61); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[5]);}
    if (LEVEL_SET_TYPE == 7 && PARTITION_MODE == 1) { glCallList(Level_Set_71); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[6]);}
    if (LEVEL_SET_TYPE == 8 && PARTITION_MODE == 1) { glCallList(Level_Set_81); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[7]);}

    if (LEVEL_SET_TYPE == 1 && PARTITION_MODE == 2) { glCallList(Dodecahedron); glCallList(Level_Set_12); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[0]);}
    if (LEVEL_SET_TYPE == 2 && PARTITION_MODE == 2) { glCallList(Level_Set_22); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[1]);}
    if (LEVEL_SET_TYPE == 3 && PARTITION_MODE == 2) { glCallList(Level_Set_32); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[2]);}
    if (LEVEL_SET_TYPE == 4 && PARTITION_MODE == 2) { glCallList(Level_Set_42); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[3]);}
    if (LEVEL_SET_TYPE == 5 && PARTITION_MODE == 2) { glCallList(Level_Set_52); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[4]);}
    if (LEVEL_SET_TYPE == 6 && PARTITION_MODE == 2) { glCallList(Level_Set_62); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[5]);}
    if (LEVEL_SET_TYPE == 7 && PARTITION_MODE == 2) { glCallList(Level_Set_72); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[6]);}
    if (LEVEL_SET_TYPE == 8 && PARTITION_MODE == 2) { glCallList(Level_Set_82); glColor3f(0.8,0.8,0.8); renderText(int(0.01*width()),int(0.02*height()),vec[7]);}

    //Interaction Hints
    glColor3f(0.0,1.0,0.0); renderText(int(0.02*width()),int(0.99*height()),"DoubleClick:");
    glColor3f(1.0,1.0,0.0); renderText(int(0.07*width()),int(0.99*height()),"Animate Object");
    glColor3f(0.0,1.0,0.0); renderText(int(0.44*width()),int(0.99*height()),"Move Mouse:");
    glColor3f(1.0,1.0,0.0); renderText(int(0.49*width()),int(0.99*height()),"Rotate Object");
    glColor3f(0.0,1.0,0.0); renderText(int(0.92*width()),int(0.99*height()),"Wheel:");
    glColor3f(1.0,1.0,0.0); renderText(int(0.95*width()),int(0.99*height()),"Scale Object");
 }

//Slot-Functions
    void GL_Widget::Level_Set_Type(GLuint levs)     {LEVEL_SET_TYPE     = levs; updateGL();}
    void GL_Widget::Partition_Mode(GLuint part)     {PARTITION_MODE     = part; updateGL();}
    void GL_Widget::Polygon_Mode(GLuint polg)       {POLYGON_MODE       = polg; updateGL();}
    void GL_Widget::Projection_Mode(GLuint proj)    {PROJECTION_MODE    = proj; updateGL();}
    void GL_Widget::Blending_Mode(GLuint bldg)      {BLENDING_MODE      = bldg; updateGL();}

//Timer
void GL_Widget::timerEvent(QTimerEvent *event) {if(event->timerId() == timerId) {t += 0.25; updateGL();}}


//Mouse Handlers
void GL_Widget::mouseDoubleClickEvent(QMouseEvent *event)
{   if (event->button() != Qt::LeftButton )	return;
    if( timerId == 0)	{timerId = startTimer(10);}
    else 		{killTimer(timerId); timerId = 0;}
}

void GL_Widget::mousePressEvent(QMouseEvent *event) {lastPos = event->pos();}

void GL_Widget::mouseMoveEvent(QMouseEvent *event)
{   GLdouble DX = GLdouble(event->x()-lastPos.x())/width();
    GLdouble DY = GLdouble(event->y()-lastPos.y())/height();
    if (event->buttons() & Qt::LeftButton)  {rotX += 180*DY; rotY += 180*DX; updateGL();}
    else
    if (event->buttons() & Qt::RightButton) {rotX += 180*DY; rotZ += 180*DX; updateGL();}
    lastPos = event->pos();
}

void GL_Widget::wheelEvent(QWheelEvent *event)
{   int numDegrees = event->delta()/32;
    double numSteps = numDegrees/50.0;
    zoom(pow(ZoomFactor, numSteps));
}

void GL_Widget::zoom(double zoomFactor) {currentScaling *= zoomFactor; updateGL();}

GLvoid GL_Widget::vMarchingCubes(GLint levelset, GLint partition)
{
    GLfloat fTv;    //Targetvalue
    //Select Level-Set-Function
    switch ( levelset )
    {
        case 1: { fSample = &GL_Widget::fSample1; fTv = 9.300; break;}
        case 2: { fSample = &GL_Widget::fSample2; fTv = 3.000; break;}
        case 3: { fSample = &GL_Widget::fSample3; fTv = 0.015; break;}
        case 4: { fSample = &GL_Widget::fSample4; fTv = 3.000; break;}
        case 5: { fSample = &GL_Widget::fSample5; fTv = 3.000; break;}
        case 6: { fSample = &GL_Widget::fSample6; fTv = 0.000; break;}
        case 7: { fSample = &GL_Widget::fSample7; fTv = 1.000; break;}
        case 8: { fSample = &GL_Widget::fSample8; fTv = 0.000; break;}
        default:    break;
    }
    //Select Partition Mode
     switch ( partition )
    {
        case 1: { vMarchCube = &GL_Widget::vMarchCube1; break;}
        case 2: { vMarchCube = &GL_Widget::vMarchCube2; break;}
        default:    break;
    }

    //Start Computation
    for(GLint iX = 0; iX < iDataSetSize; iX++)
    for(GLint iY = 0; iY < iDataSetSize; iY++)
    for(GLint iZ = 0; iZ < iDataSetSize; iZ++)
    {
        (this->*vMarchCube)(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, fTv);
    }
}


/*
fGetOffset finds the approximate point of intersection of the surface
between two points with the values fValue1 and fValue2
*/
GLfloat GL_Widget::fGetOffset(const GLfloat &fValue1, const GLfloat &fValue2, const GLfloat &fValueDesired)
{
        GLdouble fDelta = fValue2 - fValue1;
        if(fDelta == 0.0) { return 0.5; }
        return (fValueDesired - fValue1)/fDelta;
}

//vGetColor generates a color from a given position and normal of a point
GLvoid GL_Widget::vGetColor(GLvector &rfColor, GLvector &rfPosition, GLvector &rfNormal)
{
        GLfloat fX = rfNormal.fX;
        GLfloat fY = rfNormal.fY;
        GLfloat fZ = rfNormal.fZ;
        rfColor.fX = (fX > 0.0 ? fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0);
        rfColor.fY = (fY > 0.0 ? fY : 0.0) + (fZ < 0.0 ? -0.5*fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0);
        rfColor.fZ = (fZ > 0.0 ? fZ : 0.0) + (fX < 0.0 ? -0.5*fX : 0.0) + (fY < 0.0 ? -0.5*fY : 0.0);
}

GLvoid GL_Widget::vNormalizeVector(GLvector &rfVectorResult, GLvector &rfVectorSource)
{
        GLfloat fOldLength;
        GLfloat fScale;

        fOldLength = sqrt( (rfVectorSource.fX * rfVectorSource.fX) +
                            (rfVectorSource.fY * rfVectorSource.fY) +
                            (rfVectorSource.fZ * rfVectorSource.fZ) );

        if(fOldLength == 0.0)
        {
                rfVectorResult.fX = rfVectorSource.fX;
                rfVectorResult.fY = rfVectorSource.fY;
                rfVectorResult.fZ = rfVectorSource.fZ;
        }
        else
        {
                fScale = 1.0/fOldLength;
                rfVectorResult.fX = rfVectorSource.fX*fScale;
                rfVectorResult.fY = rfVectorSource.fY*fScale;
                rfVectorResult.fZ = rfVectorSource.fZ*fScale;
        }
}

void GL_Widget::axes(const GLfloat& x, const GLfloat& y, const GLfloat& z, const GLfloat& red, const GLfloat& green, const GLfloat& blue) const
{ glDisable(GL_LIGHTING);
  glColor3f(red,green,blue);
  glLineWidth(1);
  glBegin(GL_LINES);
    glVertex3f(-x,0.0,0.0); glVertex3f( x,0.0,0.0);
    glVertex3f(0.0,-y,0.0); glVertex3f(0.0, y,0.0);
    glVertex3f(0.0,0.0,-z); glVertex3f(0.0,0.0, z);
  glEnd();
  glEnable(GL_LIGHTING);
}


/* ------------------- Sample Level-Set Functions ------------------- */

//fSample1 shows a Potential , 20 pos. pointcharges, positioned by a Dodecahedron
GLfloat GL_Widget::fSample1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{    GLfloat fHeight = 0.0, dX = 0.5-fX, dY = 0.5-fY, dZ = 0.5-fZ;

     for(size_t i = 0; i < sizeof(D)/sizeof(D[0]); i++)
     {
        fHeight +=  1.0/sqrt((dX+D[i][0])*(dX+D[i][0])+(dY+D[i][1])*(dY+D[i][1])+(dZ+D[i][2])*(dZ+D[i][2]));
     }
    return 0.15*fHeight;
}

//fSample2 shows a Toroidal Surface (phantasy!)
GLfloat GL_Widget::fSample2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat a = 2.5, fscale = 7.0, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return (pow(sqrt(pow(dX,2)+pow(dY,2))-a,2) +pow(dZ,2))*(pow(sqrt(pow(dY,2)+pow(dZ,2))-a,2) +pow(dX,2))*(pow(sqrt(pow(dZ,2)+pow(dX,2))-a,2) +pow(dY,2));
}

//fSample3 shows a Double Torus (local.wasp.uwa.edu.au/~pbourke/geometry/doubletorus/)
GLfloat GL_Widget::fSample3(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.25, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return pow(dX,8)+pow(dX,4)-2*pow(dX,6)-2*pow(dX,2)*pow(dY,2)+2*pow(dX,4)*pow(dY,2)+pow(dY,4)+pow(dZ,2);
}

//fSample4 shows a Chmutov-Surface-1 (http://mathworld.wolfram.com)
GLfloat GL_Widget::fSample4(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.5, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return 8*(pow(dX,2)+pow(dY,2)+pow(dZ,2))-8*(pow(dX,4)+pow(dY,4)+pow(dZ,4));
}

//fSample5 shows a Chmutov-Surface-2 (http://mathworld.wolfram.com)
GLfloat GL_Widget::fSample5(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 2.5, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return 2*( pow(dX,2)*pow(3-4*pow(dX,2),2)+pow(dY,2)*pow(3-4*pow(dY,2),2)+pow(dZ,2)*pow(3-4*pow(dZ,2),2) );
}

//fSample6 shows a Klein Bottle (http://mathworld.wolfram.com)
GLfloat GL_Widget::fSample6(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 7.5, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return (pow(dX,2)+pow(dY,2)+pow(dZ,2)+2*dY-1)*(pow(pow(dX,2)+pow(dY,2)+pow(dZ,2)-2*dY-1,2)-8*pow(dZ,2))+16*dX*dZ*(pow(dX,2)+pow(dY,2)+pow(dZ,2)-2*dY-1);
}

GLuint GL_Widget::fac(GLuint n)
{
    if(n <= 0) return 1;
    return n*fac(n-1);
}
//Unnormalized Radial Function
GLfloat GL_Widget::R(GLuint n, GLuint l, GLfloat r)
{
    return 2.0/pow(n,2)*pow(2.0*r/n,l)*exp(-r/n)*fac(n+l)*laguerre(n-l-1,2*l+1,2.0*r/n);
}

//Unnormalized Eigen Function
complex<GLfloat> GL_Widget::Psi(GLuint n, GLint l, GLint m, GLfloat r, GLfloat theta, GLfloat phi)
{
    return R(n,l,r)*spherical_harmonic(l,m,theta,phi);
}

//fSample7 shows Unnormalized Hydrogen Superposition-State (n=4, l=3, m=0 & n=5, l=4, m=0)(any book to Quantum Mechanics)
GLfloat GL_Widget::fSample7(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 320.0, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //Polar- ---> Cartesian Coordinates
    complex<GLfloat> psi(
                            Psi(4,3,0,sqrt(pow(dX,2)+pow(dY,2)+pow(dZ,2)),atan(sqrt(pow(dX,2)+pow(dY,2))/dZ),atan(dY/dX))+
                            Psi(5,4,0,sqrt(pow(dX,2)+pow(dY,2)+pow(dZ,2)),atan(sqrt(pow(dX,2)+pow(dY,2))/dZ),atan(dY/dX))
                        );

    return (10^6)*real(psi*conj(psi));
}

//fSample8 shows Minkowski Space (Roger Penrose, The Road To Reality, p.423)
GLfloat GL_Widget::fSample8(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
    GLfloat fscale = 7.0, dX = fscale*(0.5-fX), dY = fscale*(0.5-fY), dZ = fscale*(0.5-fZ);
    //algebraic equation
    return (pow(dX,2)-pow(dY,2)-pow(dZ,2)-2)*(pow(dX,2)-pow(dY,2)-pow(dZ,2)+2)*(pow(dX,2)-pow(dY,2)-pow(dZ,2)-4)*(pow(dX,2)-pow(dY,2)-pow(dZ,2)+4)
            *(pow(dX,2)-pow(dY,2)-pow(dZ,2));
}


//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid GL_Widget::vGetNormal(GLvector &rfNormal, const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ)
{
        rfNormal.fX = (this->*fSample)(fX-0.01, fY, fZ) - (this->*fSample)(fX+0.01, fY, fZ);
        rfNormal.fY = (this->*fSample)(fX, fY-0.01, fZ) - (this->*fSample)(fX, fY+0.01, fZ);
        rfNormal.fZ = (this->*fSample)(fX, fY, fZ-0.01) - (this->*fSample)(fX, fY, fZ+0.01);
        vNormalizeVector(rfNormal, rfNormal);
}

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid GL_Widget::vMarchCube1(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLvector sColor;
        GLfloat afCubeValue[8];
        GLvector asEdgeVertex[12];
        GLvector asEdgeNorm[12];

        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(fX + a2fVertexOffset[iVertex][0]*fScale,fY + a2fVertexOffset[iVertex][1]*fScale,fZ + a2fVertexOffset[iVertex][2]*fScale);
        }

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
                if(afCubeValue[iVertexTest] <= fTv)     iFlagIndex |= 1<<iVertexTest;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

        //If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }

        //Find the point of intersection of the surface with each edge
        //Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ],afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTv);

                asEdgeVertex[iEdge].fX = fX + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                asEdgeVertex[iEdge].fY = fY + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                asEdgeVertex[iEdge].fZ = fZ + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
}


        //Draw the triangles that were found.  There can be up to five per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
            if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];

                vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                glColor4f(sColor.fX, sColor.fY, sColor.fZ, 0.6);
                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}

//vMarchCube2 performs the Marching Tetrahedrons algorithm on a single cube by making six calls to vMarchTetrahedron
GLvoid GL_Widget::vMarchCube2(const GLfloat &fX, const GLfloat &fY, const GLfloat &fZ, const GLfloat &fScale, const GLfloat &fTv)
{
        GLint iVertex, iTetrahedron, iVertexInACube;
        GLvector asCubePosition[8];
        GLfloat  afCubeValue[8];
        GLvector asTetrahedronPosition[4];
        GLfloat  afTetrahedronValue[4];

        //Make a local copy of the cube's corner positions
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
                asCubePosition[iVertex].fX = fX + a2fVertexOffset[iVertex][0]*fScale;
                asCubePosition[iVertex].fY = fY + a2fVertexOffset[iVertex][1]*fScale;
                asCubePosition[iVertex].fZ = fZ + a2fVertexOffset[iVertex][2]*fScale;
        }

        //Make a local copy of the cube's corner values
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = (this->*fSample)(asCubePosition[iVertex].fX,asCubePosition[iVertex].fY,asCubePosition[iVertex].fZ);
        }

        for(iTetrahedron = 0; iTetrahedron < 6; iTetrahedron++)
        {
            for(iVertex = 0; iVertex < 4; iVertex++)
            {
                iVertexInACube = a2iTetrahedronsInACube[iTetrahedron][iVertex];
                asTetrahedronPosition[iVertex].fX = asCubePosition[iVertexInACube].fX;
                asTetrahedronPosition[iVertex].fY = asCubePosition[iVertexInACube].fY;
                asTetrahedronPosition[iVertex].fZ = asCubePosition[iVertexInACube].fZ;
                afTetrahedronValue[iVertex] = afCubeValue[iVertexInACube];
            }
            vMarchTetrahedron(asTetrahedronPosition, afTetrahedronValue, fTv);
        }
}


//vMarchTetrahedron performs the Marching Tetrahedrons algorithm on a single tetrahedron
GLvoid GL_Widget::vMarchTetrahedron(GLvector *pasTetrahedronPosition, GLfloat *pafTetrahedronValue, GLfloat fTv)
{
        GLint iEdge, iVert0, iVert1, iEdgeFlags, iTriangle, iCorner, iVertex, iFlagIndex = 0;
        GLfloat fOffset, fInvOffset;
        GLvector asEdgeVertex[6];
        GLvector asEdgeNorm[6];
        GLvector sColor;

        //Find which vertices are inside of the surface and which are outside
        for(iVertex = 0; iVertex < 4; iVertex++)
        {
                if(pafTetrahedronValue[iVertex] <= fTv)     iFlagIndex |= 1<<iVertex;
        }

        //Find which edges are intersected by the surface
        iEdgeFlags = aiTetrahedronEdgeFlags[iFlagIndex];

        //If the tetrahedron is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0)
        {
                return;
        }
        //Find the point of intersection of the surface with each edge
        // Then find the normal to the surface at those points
        for(iEdge = 0; iEdge < 6; iEdge++)
        {
            //if there is an intersection on this edge
            if(iEdgeFlags & (1<<iEdge))
            {
                iVert0 = a2iTetrahedronEdgeConnection[iEdge][0];
                iVert1 = a2iTetrahedronEdgeConnection[iEdge][1];
                fOffset = fGetOffset(pafTetrahedronValue[iVert0], pafTetrahedronValue[iVert1], fTv);
                fInvOffset = 1.0 - fOffset;

                asEdgeVertex[iEdge].fX = fInvOffset*pasTetrahedronPosition[iVert0].fX  +  fOffset*pasTetrahedronPosition[iVert1].fX;
                asEdgeVertex[iEdge].fY = fInvOffset*pasTetrahedronPosition[iVert0].fY  +  fOffset*pasTetrahedronPosition[iVert1].fY;
                asEdgeVertex[iEdge].fZ = fInvOffset*pasTetrahedronPosition[iVert0].fZ  +  fOffset*pasTetrahedronPosition[iVert1].fZ;

                vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].fX, asEdgeVertex[iEdge].fY, asEdgeVertex[iEdge].fZ);
            }
        }
        //Draw the triangles that were found.  There can be up to 2 per tetrahedron
        for(iTriangle = 0; iTriangle < 2; iTriangle++)
        {
            if(a2iTetrahedronTriangles[iFlagIndex][3*iTriangle] < 0)    break;

            for(iCorner = 0; iCorner < 3; iCorner++)
            {
                iVertex = a2iTetrahedronTriangles[iFlagIndex][3*iTriangle+iCorner];

                vGetColor(sColor, asEdgeVertex[iVertex], asEdgeNorm[iVertex]);
                glColor4f(sColor.fX, sColor.fY, sColor.fZ, 0.6);
                glNormal3f(asEdgeNorm[iVertex].fX,   asEdgeNorm[iVertex].fY,   asEdgeNorm[iVertex].fZ);
                glVertex3f(asEdgeVertex[iVertex].fX, asEdgeVertex[iVertex].fY, asEdgeVertex[iVertex].fZ);
            }
        }
}
