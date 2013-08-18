#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class Application
{
    //_____ Construction / Destruction __________
public:
    Application();


    //_____ Methods __________
public:
    bool init(int argc, char** argv);
    void run();

    inline static Application* getPtr()
    {
        return m_pInstance;
    }

private:
    void initOpenGL(int argc, char** argv);

    void display();

    static void displayCallback();


    //_____ Attributes __________
private:
    static Application* m_pInstance;
};

#endif
