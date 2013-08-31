#ifndef _FRONT_VIEW_H_
#define _FRONT_VIEW_H_

#include "TextureView.h"
#include <OpenNI.h>
#include <NiTE.h>


class FrontView: public TextureView
{
    //_____ Construction / Destruction __________
public:
    FrontView();
    virtual ~FrontView();


    //_____ Methods __________
public:
    virtual void init(unsigned int windowWidth, unsigned int windowHeight,
                      unsigned int x, unsigned int y, unsigned int width,
                      unsigned int height
                      DEVELOPMENT_PARAMETER(bool layoutDebugging = false));


    //_____ Attributes __________
protected:
};

#endif
