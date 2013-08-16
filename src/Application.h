#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class Application
{
    //_____ Methods __________
public:
    bool init(int argc, char** argv);
    void run();

private:
    void initOpenGL(int argc, char** argv);
};

#endif
