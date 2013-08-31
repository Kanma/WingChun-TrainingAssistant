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
: m_window(0), m_phase(PHASE_USER_DETECTION)
#ifdef DEVELOPMENT_FEATURES
, m_bLayoutDebugging(false)
#endif
{
    m_pInstance = this;

    pthread_mutex_init (&m_capture_mutex, 0);
}


Application::~Application()
{
    destroy();
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

void Application::destroy()
{
    if (m_window != 0)
        glutDestroyWindow(m_window);

    if (m_device.isValid())
    {
        kinect_led_options options = KINECT_LED_OFF;
        m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));
    }

    m_userTracker.destroy();

    nite::NiTE::shutdown();

    if (m_videoStream.isValid())
    {
        m_videoStream.stop();
        m_videoStream.destroy();
    }

    m_device.close();

    openni::OpenNI::shutdown();
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

    // Change the LED status
    kinect_led_options options = KINECT_LED_YELLOW;
    m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));

    // Initialize the video stream
    rc = m_videoStream.create(m_device, openni::SENSOR_COLOR);
    if (rc != openni::STATUS_OK)
    {
        printf("Failed to create the video stream\n%s\n", openni::OpenNI::getExtendedError());
        return false;
    }

    rc = m_videoStream.addNewFrameListener(this);
    if (rc != openni::STATUS_OK)
    {
        printf("Failed to register the listener for the video stream\n%s\n",
               openni::OpenNI::getExtendedError());
        return false;
    }

    m_videoStream.start();

    // Initialize the tracker
    nite::NiTE::initialize();

    if (m_userTracker.create(&m_device) != nite::STATUS_OK)
        return false;

    m_userTracker.addNewFrameListener(this);

    return true;
}

//----------------------------------------------

void Application::initOpenGL(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);

    m_window = glutCreateWindow("Wing Chun Training Assistant");

    // glutFullScreen();

    glutSetCursor(GLUT_CURSOR_NONE);

    glutKeyboardFunc(keyPressedCallback);
    glutDisplayFunc(displayCallback);
    glutIdleFunc(glutPostRedisplay);

    glEnableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
}

//----------------------------------------------

void Application::display()
{
    switch (m_phase)
    {
        case PHASE_USER_DETECTION:     phaseUserDetection_process(); break;
        case PHASE_SKELETON_DETECTION: phaseSkeletonDetection_process(); break;
        default:                       break;
    }


    // Clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Ask each view to display itself
    m_frontView.display();
    m_topView.display();

    // Swap the OpenGL display buffers
    glutSwapBuffers();
}

//----------------------------------------------

void Application::phaseUserDetection_process()
{
#ifdef DEVELOPMENT_FEATURES
    if (!m_bLayoutDebugging)
    {
#endif
    pthread_mutex_lock(&m_capture_mutex);

    if (!m_userTrackerFrame.isValid() || !m_videoFrame.isValid())
    {
        pthread_mutex_unlock(&m_capture_mutex);
        return;
    }

    // Initialise the view if necessary
    if (!m_frontView.isInitialised())
    {
        unsigned int width  = WINDOW_WIDTH;
        unsigned int height = width * m_videoFrame.getHeight() / m_videoFrame.getWidth();

        if (height > WINDOW_HEIGHT)
        {
            height = WINDOW_HEIGHT;
            width  = height * m_videoFrame.getWidth() / m_videoFrame.getHeight();
        }

        m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                         (WINDOW_WIDTH - width) / 2,
                         (WINDOW_HEIGHT - height) / 2,
                         width, height);

        kinect_led_options options = KINECT_LED_BLINK_GREEN;
        m_device.setProperty(KINECT_PROPERTY_LED_STATUS, &options, sizeof(kinect_led_options));
    }

    // When one user is detected, change the phase
    const nite::Array<nite::UserData>& users = m_userTrackerFrame.getUsers();
    if (users.getSize() != 0)
    {
        m_phase = PHASE_SKELETON_DETECTION;

        m_frontView.reset();

        // TODO: Start the skeleton detection

        m_userTrackerFrame.release();
    }
    else
    {
        // Update the view
        m_frontView.setVideoFrame(&m_videoFrame);
        m_videoFrame.release();
    }

    pthread_mutex_unlock(&m_capture_mutex);

#ifdef DEVELOPMENT_FEATURES
    }
    else
    {
        if (!m_frontView.isInitialised())
        {
            unsigned int width  = WINDOW_WIDTH;
            unsigned int height = width * 480 / 640;

            if (height > WINDOW_HEIGHT)
            {
                height = WINDOW_HEIGHT;
                width  = height * 640 / 480;
            }

            m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                             (WINDOW_WIDTH - width) / 2,
                             (WINDOW_HEIGHT - height) / 2,
                             width, height, true);
        }

        static int counter = 0;
        ++counter;
        if (counter == 100)
        {
            m_phase = PHASE_SKELETON_DETECTION;
            m_frontView.reset();
        }
    }
#endif
}

//----------------------------------------------

void Application::phaseSkeletonDetection_process()
{
#ifdef DEVELOPMENT_FEATURES
    if (!m_bLayoutDebugging)
    {
#endif
    pthread_mutex_lock(&m_capture_mutex);

    if (!m_userTrackerFrame.isValid() || !m_videoFrame.isValid())
    {
        pthread_mutex_unlock(&m_capture_mutex);
        return;
    }

    // Initialise the views if necessary
    if (!m_frontView.isInitialised())
    {
        unsigned int width  = WINDOW_WIDTH / 2 - 2;
        unsigned int height = width * m_videoFrame.getHeight() / (m_videoFrame.getWidth() / 2);

        if (height > WINDOW_HEIGHT)
        {
            height = WINDOW_HEIGHT - 2 / 2;
            width  = height * (m_videoFrame.getWidth() / 2) / m_videoFrame.getHeight();
        }

        m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                         WINDOW_WIDTH / 2 - 1 - width,
                         (WINDOW_HEIGHT - height) / 2,
                         width, height);

        m_topView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                       WINDOW_WIDTH / 2 + 1,
                       (WINDOW_HEIGHT - height) / 2,
                       width, height);
    }

    // TODO: When the skeleton detection is done, change the phase
    // if (....isValid())
    // {
    //     m_phase = ...;
    // }
    // else
    {
        // Update the views
        m_frontView.setVideoFrame(&m_videoFrame);

        openni::VideoFrameRef depthFrame = m_userTrackerFrame.getDepthFrame();
        m_topView.setDepthFrame(&depthFrame);

        m_videoFrame.release();
        m_userTrackerFrame.release();
    }

    pthread_mutex_unlock(&m_capture_mutex);

#ifdef DEVELOPMENT_FEATURES
    }
    else
    {
        if (!m_frontView.isInitialised())
        {
            unsigned int width  = WINDOW_WIDTH / 2 - 2;
            unsigned int height = width * 480 / 320;

            if (height > WINDOW_HEIGHT)
            {
                height = WINDOW_HEIGHT - 2 / 2;
                width  = height * 320 / 480;
            }

            m_frontView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                             WINDOW_WIDTH / 2 - 1 - width,
                             (WINDOW_HEIGHT - height) / 2,
                             width, height, true);

            m_topView.init(WINDOW_WIDTH, WINDOW_HEIGHT,
                           WINDOW_WIDTH / 2 + 1,
                           (WINDOW_HEIGHT - height) / 2,
                           width, height, true);
        }
    }
#endif
}

//----------------------------------------------

void Application::displayCallback()
{
    Application::getPtr()->display();
}

//----------------------------------------------

void Application::keyPressedCallback(unsigned char key, int x, int y)
{
    if (key == 27) {
        Application::getPtr()->destroy();
        exit(0);
    }
}


/************** IMPLEMENTATION OF openni::VideoStream::NewFrameListener *****************/

void Application::onNewFrame(openni::VideoStream& stream)
{
    pthread_mutex_lock(&m_capture_mutex);

    if (m_videoFrame.isValid())
        m_videoFrame.release();

    // Retrieve the current state of the user tracker
    stream.readFrame(&m_videoFrame);

    pthread_mutex_unlock(&m_capture_mutex);
}


/************** IMPLEMENTATION OF nite::UserTracker::NewFrameListener *****************/

void Application::onNewFrame(nite::UserTracker& tracker)
{
    pthread_mutex_lock(&m_capture_mutex);

    if (m_userTrackerFrame.isValid())
        m_userTrackerFrame.release();

    // Retrieve the current state of the user tracker
    m_userTracker.readFrame(&m_userTrackerFrame);

    pthread_mutex_unlock(&m_capture_mutex);
}
