#ifndef _TOP_VIEW_H_
#define _TOP_VIEW_H_

#include "TextureView.h"
#include <OpenNI.h>


class TopView: public TextureView
{
    //_____ Construction / Destruction __________
public:
    TopView();
    virtual ~TopView();


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
