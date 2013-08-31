#ifndef _TEXTURE_VIEW_H_
#define _TEXTURE_VIEW_H_

#include "Declarations.h"
#include <OpenNI.h>
#include <NiTE.h>


class TextureView
{
    //_____ Construction / Destruction __________
public:
    TextureView();
    virtual ~TextureView();


    //_____ Methods __________
public:
    inline bool isInitialised() const
    {
        return (m_windowWidth != 0);
    }

    virtual void init(unsigned int windowWidth, unsigned int windowHeight,
                      unsigned int x, unsigned int y, unsigned int width,
                      unsigned int height
                      DEVELOPMENT_PARAMETER(bool layoutDebugging = false));

    void reset();

    void setVideoFrame(openni::VideoFrameRef* pVideoFrame);
    void setDepthFrame(openni::VideoFrameRef* pDepthFrame);
    void setUserMap(const nite::UserMap& userMap);

    void display();

protected:
    void createTexture(unsigned int srcWidth, unsigned int srcHeight);


    //_____ Attributes __________
protected:
    openni::RGB888Pixel* m_pTexture;
    unsigned int         m_textureWidth;
    unsigned int         m_textureHeight;
    unsigned int         m_srcWidth;
    unsigned int         m_srcHeight;
    unsigned int         m_windowWidth;
    unsigned int         m_windowHeight;
    unsigned int         m_x;
    unsigned int         m_y;
    unsigned int         m_width;
    unsigned int         m_height;
};

#endif
