#include "FrontView.h"
#include <GLUT/glut.h>


//----------------------------------------------

FrontView::FrontView()
: m_pTexture(0), m_textureWidth(0), m_textureHeight(0), m_srcWidth(0), m_srcHeight(0)
{
}

//----------------------------------------------

FrontView::~FrontView()
{
    delete[] m_pTexture;
}

//----------------------------------------------

void FrontView::setDepthFrame(openni::VideoFrameRef* pDepthFrame)
{
    if (!m_pTexture)
    {
        m_srcWidth  = pDepthFrame->getWidth();
        m_srcHeight = pDepthFrame->getHeight();

        m_textureWidth  = ((m_srcWidth - 1) / 512 + 1) * 512;
        m_textureHeight = ((m_srcHeight - 1) / 512 + 1) * 512;;

        m_pTexture = new openni::RGB888Pixel[m_textureWidth * m_textureHeight];
    }

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

//----------------------------------------------

void FrontView::display()
{
    // Setup the projection
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, m_srcWidth, m_srcHeight, 0, -1.0, 1.0);

    // Prepare the texture
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureWidth, m_textureHeight, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, m_pTexture);

    // Display it on a quad
    glColor4f(1,1,1,1);

    glEnable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    // g_nXRes = depthFrame.getVideoMode().getResolutionX();
    // g_nYRes = depthFrame.getVideoMode().getResolutionY();
    // 
    // upper left
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);
    // upper right
    glTexCoord2f((float) m_srcWidth / (float) m_textureWidth, 0);
    glVertex2f(m_srcWidth, 0);

    // bottom right
    glTexCoord2f((float) m_srcWidth / (float) m_textureWidth,
                 (float) m_srcHeight / (float) m_textureHeight);
    glVertex2f(m_srcWidth, m_srcHeight);

    // bottom left
    glTexCoord2f(0, (float) m_srcHeight / (float) m_textureHeight);
    glVertex2f(0, m_srcHeight);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}
