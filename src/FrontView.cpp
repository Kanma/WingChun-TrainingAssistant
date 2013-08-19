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

//----------------------------------------------

void FrontView::setDepthFrame(openni::VideoFrameRef* pDepthFrame)
{
    if (!m_pTexture)
        createTexture(pDepthFrame->getWidth(), pDepthFrame->getHeight());

    memset(m_pTexture, 0, m_textureWidth * m_textureHeight * sizeof(openni::RGB888Pixel));

    const openni::DepthPixel* pDepthRow = (const openni::DepthPixel*) pDepthFrame->getData();
    openni::RGB888Pixel* pTextureRow    = m_pTexture + pDepthFrame->getCropOriginY() * m_textureWidth;
    int rowSize                         = pDepthFrame->getStrideInBytes() / sizeof(openni::DepthPixel);

    for (int y = 0; y < m_srcHeight; ++y)
    {
        const openni::DepthPixel* pDepth = pDepthRow;
        openni::RGB888Pixel* pDest       = pTextureRow + pDepthFrame->getCropOriginX();

        for (int x = 0; x < m_srcWidth; ++x, ++pDepth, ++pDest)
        {
            if (*pDepth != 0)
            {
                pDest->r = 255 * ((1.0f - (float) *pDepth) / 0xFFFF);
                pDest->g = pDest->r;
                pDest->b = pDest->r;
            }
        }

        pDepthRow   += rowSize;
        pTextureRow += m_textureWidth;
    }
}
