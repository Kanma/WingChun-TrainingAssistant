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

void FrontView::setVideoFrame(openni::VideoFrameRef* pVideoFrame,
                              const nite::UserMap& userMap)
{
    if (!m_pTexture)
        createTexture(pVideoFrame->getWidth(), pVideoFrame->getHeight());
    
    memset(m_pTexture, 0, m_textureWidth * m_textureHeight * sizeof(openni::RGB888Pixel));
    
    const openni::RGB888Pixel* pSrcRow = (const openni::RGB888Pixel*) pVideoFrame->getData();
    const nite::UserId* pMaskRow       = (const nite::UserId*) userMap.getPixels();
    openni::RGB888Pixel* pTextureRow   = m_pTexture + pVideoFrame->getCropOriginY() * m_textureWidth;
    int rowSize                        = pVideoFrame->getStrideInBytes() / sizeof(openni::RGB888Pixel);
    int maskRowSize                    = userMap.getStride() / sizeof(nite::UserId);
    
    for (int y = 0; y < m_srcHeight; ++y)
    {
        const openni::RGB888Pixel* pSrc = pSrcRow;
        const nite::UserId* pMask       = pMaskRow;
        openni::RGB888Pixel* pDest      = pTextureRow + pVideoFrame->getCropOriginX();

        nite::UserId previous = 0;
        for (int x = 0; x < m_srcWidth; ++x, ++pSrc, ++pMask, ++pDest)
        {
            if (*pMask != 0)
            {
                if (previous == 0)
                {
                    pDest->r = 0xFF;
                    pDest->g = 0x00;
                    pDest->b = 0x00;
                }
                else
                {
                    *pDest = *pSrc;
                }
            }
            else
            {
                if (previous != 0)
                {
                    pDest->r = 0xFF;
                    pDest->g = 0x00;
                    pDest->b = 0x00;
                }
                else
                {
                    pDest->r = pSrc->r >> 2;
                    pDest->g = pSrc->g >> 2;
                    pDest->b = pSrc->b >> 2;
                }
            }

            previous = *pMask;
        }
    
        pSrcRow     += rowSize;
        pMaskRow    += maskRowSize;
        pTextureRow += m_textureWidth;
    }
}

//----------------------------------------------

void FrontView::setVideoFrame(openni::VideoFrameRef* pVideoFrame)
{
    if (!m_pTexture)
        createTexture(pVideoFrame->getWidth(), pVideoFrame->getHeight());

    memset(m_pTexture, 0, m_textureWidth * m_textureHeight * sizeof(openni::RGB888Pixel));

    const openni::RGB888Pixel* pSrcRow = (const openni::RGB888Pixel*) pVideoFrame->getData();
    openni::RGB888Pixel* pTextureRow   = m_pTexture + pVideoFrame->getCropOriginY() * m_textureWidth;
    int rowSize                        = pVideoFrame->getStrideInBytes() / sizeof(openni::RGB888Pixel);

    for (int y = 0; y < m_srcHeight; ++y)
    {
        const openni::RGB888Pixel* pSrc = pSrcRow;
        openni::RGB888Pixel* pDest      = pTextureRow + pVideoFrame->getCropOriginX();

        for (int x = 0; x < m_srcWidth; ++x, ++pSrc, ++pDest)
            *pDest = *pSrc;

        pSrcRow     += rowSize;
        pTextureRow += m_textureWidth;
    }
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
