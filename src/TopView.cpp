#include "TopView.h"
#include <GLUT/glut.h>


//----------------------------------------------

TopView::TopView()
{
}

//----------------------------------------------

TopView::~TopView()
{
}

//----------------------------------------------

void TopView::init(unsigned int windowWidth, unsigned int windowHeight,
                   unsigned int x, unsigned int y, unsigned int width,
                   unsigned int height
                   DEVELOPMENT_PARAMETER(bool layoutDebugging))
{
    TextureView::init(windowWidth, windowHeight, x, y, width, height
                      DEVELOPMENT_PARAMETER(layoutDebugging));

#ifdef DEVELOPMENT_FEATURES
    if (layoutDebugging)
    {
        createTexture(480, 640);

        openni::RGB888Pixel* pTextureRow = m_pTexture;

        for (int y = 0; y < 640; ++y)
        {
            openni::RGB888Pixel* pDest = pTextureRow;

            for (int x = 0; x < 480; ++x, ++pDest)
            {
                pDest->r = 0;
                pDest->g = 0;
                pDest->b = 255;
            }

            pTextureRow += m_textureWidth;
        }
    }
#endif
}

//----------------------------------------------

void TopView::setUserMap(const nite::UserMap& userMap)
{
    if (!m_pTexture)
        createTexture(userMap.getWidth(), userMap.getHeight());

    memset(m_pTexture, 0, m_textureWidth * m_textureHeight * sizeof(openni::RGB888Pixel));

    const nite::UserId* pSrcRow      = (const nite::UserId*) userMap.getPixels();
    openni::RGB888Pixel* pTextureRow = m_pTexture;
    int rowSize                      = userMap.getStride() / sizeof(nite::UserId);

    for (int y = 0; y < m_srcHeight; ++y)
    {
        const nite::UserId* pSrc   = pSrcRow;
        openni::RGB888Pixel* pDest = pTextureRow;

        for (int x = 0; x < m_srcWidth; ++x, ++pSrc, ++pDest)
        {
            if (*pSrc != 0)
            {
                pDest->r = 0xFF;
                pDest->g = 0xFF;
                pDest->b = 0xFF;
            }
        }

        pSrcRow     += rowSize;
        pTextureRow += m_textureWidth;
    }
}
