# -------------------------------------------------
# Project created by QtCreator 2009-07-02T20:31:32
# -------------------------------------------------
QT += opengl
TARGET = Qt_MARCHING_CUBES
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    marching_cubes.cpp
HEADERS += mainwindow.h \
    marching_cubes.h
FORMS += 
macx {
    INCLUDEPATH += /usr/local/include
}

win32 {
    INCLUDEPATH += "C:/Program Files/boost/boost_1_57_0"
}
#LIBS += -lglut -lGLU

DISTFILES += \
    README.md \
    qt_marching_cubes_screenshot.png \
    qt_marching_cubes_screenshot_2.png
