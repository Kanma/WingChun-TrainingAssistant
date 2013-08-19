#include "Application.h"
#include <GLUT/glut.h>
#include <OpenNI.h>
#include <FreenectDriver/Properties.h>
#include <SimpleOpt.h>
#include <iostream>

using namespace std;


/************************************** CONSTANTS ***************************************/

const int WINDOW_WIDTH  = 1280;
const int WINDOW_HEIGHT = 768;

// The valid options
enum
{
    OPT_HELP,

#ifdef DEVELOPMENT_FEATURES
    OPT_LAYOUT_DEBUGGING,
#endif
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,             "-h",                   SO_NONE },
    { OPT_HELP,             "--help",               SO_NONE },

#ifdef DEVELOPMENT_FEATURES
    { OPT_LAYOUT_DEBUGGING, "--layout-debugging",   SO_NONE },
#endif

    SO_END_OF_OPTIONS
};


/********************************** STATIC ATTRIBUTES ***********************************/

Application* Application::m_pInstance = 0;


/***************************** CONSTRUCTION / DESTRUCTION *******************************/

Application::Application()
#ifdef DEVELOPMENT_FEATURES
: m_bLayoutDebugging(false)
#endif
{
    m_pInstance = this;
}


Application::~Application()
{
    if (m_device.isValid())
    {
        kinect_led_options options = KINECT_LED_OFF;
        m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));
    }
}


/************************************** METHODS *****************************************/

bool Application::init(int argc, char** argv)
{
    // Parse the command-line parameters
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    return 0;

#ifdef DEVELOPMENT_FEATURES
                case OPT_LAYOUT_DEBUGGING:
                    m_bLayoutDebugging = true;
                    break;
#endif
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return false;
        }
    }


#ifdef DEVELOPMENT_FEATURES
    if (!m_bLayoutDebugging)
    {
#endif

    if (!initNiTE())
        return false;

#ifdef DEVELOPMENT_FEATURES
    }
#endif

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

    kinect_led_options options = KINECT_LED_YELLOW;
    m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));

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
    glutIdleFunc(glutPostRedisplay);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}


//----------------------------------------------

void Application::display()
{
#ifdef DEVELOPMENT_FEATURES
    if (!m_bLayoutDebugging)
    {
#endif
    // Retrieve the current state of the user tracker
    nite::UserTrackerFrameRef userTrackerFrame;
    m_userTracker.readFrame(&userTrackerFrame);

    // Retrieve the depth frame
    openni::VideoFrameRef depthFrame = userTrackerFrame.getDepthFrame();

    // Initialise the views if necessary
    if (!m_frontView.isInitialised())
    {
        unsigned int width  = WINDOW_WIDTH * 2 / 3 - 2;
        unsigned int height = width * depthFrame.getHeight() / depthFrame.getWidth();

        m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT, 1, (WINDOW_HEIGHT - height) / 2,
                         width, height);

        width = WINDOW_WIDTH / 3 - 2;

        m_topView.init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH - width - 1,
                       (WINDOW_HEIGHT - height) / 2, width, height);

        kinect_led_options options = KINECT_LED_BLINK_GREEN;
        m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));
    }

    // Update the views
    m_frontView.setDepthFrame(&depthFrame);

#ifdef DEVELOPMENT_FEATURES
    }
    else
    {
        if (!m_frontView.isInitialised())
        {
            unsigned int width  = WINDOW_WIDTH * 2 / 3;
            unsigned int height = width * 480 / 640;

            m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT, 0, (WINDOW_HEIGHT - height) / 2,
                             width, height, true);

            width = WINDOW_WIDTH / 3 - 2;

            m_topView.init(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_WIDTH - width - 1,
                           (WINDOW_HEIGHT - height) / 2, width, height, true);
        }
    }
#endif

    // Clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Ask each view to display itself
    m_frontView.display();
    m_topView.display();

    // Swap the OpenGL display buffers
    glutSwapBuffers();
}

//----------------------------------------------

void Application::displayCallback()
{
    Application::getPtr()->display();
}
