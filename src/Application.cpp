#include "Application.h"
#include <GLUT/glut.h>
#include <OpenNI.h>


const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 768;


Application* Application::m_pInstance = 0;


//----------------------------------------------

Application::Application()
{
    m_pInstance = this;
}

//----------------------------------------------

bool Application::init(int argc, char** argv)
{
    if (!initNiTE())
        return false;

    initOpenGL(argc, argv);

    return true;
}

//----------------------------------------------

void Application::run()
{
    glutMainLoop();
}

//----------------------------------------------

bool Application::initNiTE()
{
    // Open the OpenNI device
    openni::Status rc = openni::OpenNI::initialize();
    if (rc != openni::STATUS_OK)
    {
        printf("Failed to initialize OpenNI\n%s\n", openni::OpenNI::getExtendedError());
        return false;
    }

    rc = m_device.open(openni::ANY_DEVICE);
    if (rc != openni::STATUS_OK)
    {
        printf("Failed to open device\n%s\n", openni::OpenNI::getExtendedError());
        return false;
    }

    // Initialize the tracker
    nite::NiTE::initialize();

    if (m_userTracker.create(&m_device) != nite::STATUS_OK)
        return false;

    return true;
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
    glutDisplayFunc(displayCallback);
    // glutIdleFunc(glutIdle);
}


//----------------------------------------------

void Application::display()
{
}

//----------------------------------------------

void Application::displayCallback()
{
    Application::getPtr()->display();
}
