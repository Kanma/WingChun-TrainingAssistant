#include "FrontView.h"
#include <GLUT/glut.h>


//----------------------------------------------

FrontView::FrontView()
{
}

//----------------------------------------------

FrontView::~FrontView()
{
}

//----------------------------------------------

void FrontView::init(unsigned int windowWidth, unsigned int windowHeight,
                     unsigned int x, unsigned int y, unsigned int width,
                     unsigned int height
                     DEVELOPMENT_PARAMETER(bool layoutDebugging))
{
    TextureView::init(windowWidth, windowHeight, x, y, width, height
                      DEVELOPMENT_PARAMETER(layoutDebugging));

#ifdef DEVELOPMENT_FEATURES
    if (layoutDebugging)
    {
        createTexture(640, 480);

        openni::RGB888Pixel* pTextureRow = m_pTexture;

        for (int y = 0; y < 480; ++y)
        {
            openni::RGB888Pixel* pDest = pTextureRow;

            for (int x = 0; x < 640; ++x, ++pDest)
            {
                pDest->r = 255;
                pDest->g = 0;
                pDest->b = 0;
            }

            pTextureRow += m_textureWidth;
        }
    }
#endif
}
