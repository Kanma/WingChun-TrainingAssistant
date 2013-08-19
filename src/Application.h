#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <NiTE.h>
#include "FrontView.h"
#include "TopView.h"


class Application
{
    //_____ Construction / Destruction __________
public:
    Application();
    ~Application();


    //_____ Methods __________
public:
    bool init(int argc, char** argv);
    void run();

    inline static Application* getPtr()
    {
        return m_pInstance;
    }

private:
    bool initNiTE();
    void initOpenGL(int argc, char** argv);

    void display();

    static void displayCallback();


    //_____ Attributes __________
private:
    static Application* m_pInstance;

    FrontView           m_frontView;
    TopView             m_topView;

    openni::Device      m_device;
    nite::UserTracker   m_userTracker;

#ifdef DEVELOPMENT_FEATURES
    bool                m_bLayoutDebugging;
#endif
};

#endif
