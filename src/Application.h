#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <NiTE.h>
#include <pthread.h>
#include "FrontView.h"
#include "TopView.h"


class Application: public openni::VideoStream::NewFrameListener,
                          nite::UserTracker::NewFrameListener
{
    //_____ Construction / Destruction __________
public:
    Application();
    virtual ~Application();


    //_____ Methods __________
public:
    bool init(int argc, char** argv);
    void destroy();

    void run();

    inline static Application* getPtr()
    {
        return m_pInstance;
    }

private:
    bool initNiTE();
    void initOpenGL(int argc, char** argv);

    void display();

    void phaseUserDetection_process();
    void phaseSkeletonDetection_process();

    static void displayCallback();
    static void keyPressedCallback(unsigned char key, int x, int y);


    //_____ Implementation of openni::VideoStream::NewFrameListener __________
public:
    virtual void onNewFrame(openni::VideoStream& stream);


    //_____ Implementation of nite::UserTracker::NewFrameListener __________
public:
    virtual void onNewFrame(nite::UserTracker& tracker);


    //_____ Internal types __________
private:
    enum tPhase {
        PHASE_USER_DETECTION,
        PHASE_SKELETON_DETECTION,
    };


    //_____ Attributes __________
private:
    static Application*       m_pInstance;

    tPhase                    m_phase;

    int                       m_window;
    FrontView                 m_frontView;
    TopView                   m_topView;

    openni::Device            m_device;
    openni::VideoStream       m_videoStream;
    nite::UserTracker         m_userTracker;

    openni::VideoFrameRef     m_videoFrame;
    nite::UserTrackerFrameRef m_userTrackerFrame;

    pthread_mutex_t           m_capture_mutex;

#ifdef DEVELOPMENT_FEATURES
    bool                      m_bLayoutDebugging;
#endif
};

#endif
