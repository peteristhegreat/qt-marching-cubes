Qt Marching Cubes
------

Based off of:

 - http://paulbourke.net/geometry/polygonise/
 - source: http://paulbourke.net/geometry/polygonise/Qt_MARCHING_CUBES.zip
 - Qt/OpenGL example courtesy Dr. Klaus Miltenberger.

This is here, to make it easier for someone else to get started with this project, and to look at porting the example further to Qt, and with fewer boost and glu calls.

Tested on OSX Yosemite (clang) and Windows MSVC 2010 opengl.

Example Output
----

    OpenGL Infos
    GL_VENDOR      : NVIDIA Corporation
    GL_RENDERER    : NVIDIA GeForce GT 330M OpenGL Engine
    GL_VERSION     : 2.1 NVIDIA-10.0.19 310.90.10.05b12

     Generating GL Display Lists
     (MC-Resolution) Data Set Size: 120

Delay for 30 seconds or longer...

Then shows:

![Alt text](/qt_marching_cubes_screenshot.png?raw=true "Screenshot")

![Alt text](/qt_marching_cubes_screenshot_2.png?raw=true "Alt Screenshot")

Boost Install Directions
--------------

###OSX

In the terminal run:

    brew install boost

Now in the .pro file, add

    INCLUDEPATH += /usr/local/include

###Windows

Go to http://www.boost.org/users/download/

Click download, find the latest 7z (1.57.0 at time of writing).  Note, don't get the zip version; it is very slow to unzip!

Unzip the 7z to Program Files: `C:\Program Files\boost` so you get `C:\Program Files\boost\boost_1_57_0`.

Now update the include path variable in the Qt project to be able to find boost.

    INCLUDEPATH += "C:/Program Files/boost/boost_1_57_0"

And like it mentions in the Getting Started section of boost... most of the time it is header file only additions to a project, meaning that you don't need to link a dll or anything.  It just needs to be able to find the headers at compile time.  But it does take a long time for the initial zip to unzip just because of how many files it contains!

This project only uses boost math, so it is just pulling a few header files and that's it.

http://www.boost.org/doc/libs/1_57_0/more/getting_started/windows.html#header-only-libraries

Including OpenGL
--------------
Most operating systems and compilers come with OpenGL libraries and headers.

###Mac and Linux

    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>

###Windows

http://web.eecs.umich.edu/~sugih/courses/eecs487/glut-howto/#win

    C:\Program Files (x86)\Windows Kits\8.1\Include\um\gl\{GL,GLU}.h

Then it should be accessible with

    #include "gl/GLU.h"
    #include "gl/GL.h"
