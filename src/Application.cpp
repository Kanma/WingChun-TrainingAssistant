#include "Application.h"
#include <GLUT/glut.h>


const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 768;


//----------------------------------------------

bool Application::init(int argc, char** argv)
{
    initOpenGL(argc, argv);

    return true;
}

//----------------------------------------------

void Application::run()
{
    glutMainLoop();
}

//----------------------------------------------

void Application::initOpenGL(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    glutCreateWindow("Wing Chun Training Assistant");

    // glutFullScreen();

    glutSetCursor(GLUT_CURSOR_NONE);

    // glutKeyboardFunc(glutKeyboard);
    // glutDisplayFunc(glutDisplay);
    // glutIdleFunc(glutIdle);
}
