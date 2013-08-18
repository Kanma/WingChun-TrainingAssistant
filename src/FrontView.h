#ifndef _FRONT_VIEW_H_
#define _FRONT_VIEW_H_

#include <OpenNI.h>


class FrontView
{
    //_____ Construction / Destruction __________
public:
    FrontView();
    ~FrontView();


    //_____ Methods __________
public:
    void setDepthFrame(openni::VideoFrameRef* pDepthFrame);

    void display();


    //_____ Attributes __________
private:
    openni::RGB888Pixel* m_pTexture;
    unsigned int         m_textureWidth;
    unsigned int         m_textureHeight;
    unsigned int         m_srcWidth;
    unsigned int         m_srcHeight;
};

#endif
