#include "TextureView.h"
#include <GLUT/glut.h>


//----------------------------------------------

TextureView::TextureView()
: m_pTexture(0), m_textureWidth(0), m_textureHeight(0), m_srcWidth(0), m_srcHeight(0),
  m_windowWidth(0), m_windowHeight(0), m_x(0), m_y(0), m_width(0), m_height(0)
{
}

//----------------------------------------------

TextureView::~TextureView()
{
    delete[] m_pTexture;
}

//----------------------------------------------

void TextureView::init(unsigned int windowWidth, unsigned int windowHeight,
                       unsigned int x, unsigned int y, unsigned int width,
                       unsigned int height
                       DEVELOPMENT_PARAMETER(bool layoutDebugging))
{
    m_windowWidth  = windowWidth;
    m_windowHeight = windowHeight;
    m_x            = x;
    m_y            = y;
    m_width        = width;
    m_height       = height;
}

//----------------------------------------------

void TextureView::display()
{
    glPushMatrix();

    // Setup the projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_windowWidth, m_windowHeight, 0, -1.0, 1.0);
    glTranslatef(m_x, m_y, 0);

    // Setup the model-view transformation
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

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

    //-- Upper left
    glTexCoord2f(0, 0);
    glVertex2f(0, 0);

    //-- Upper right
    glTexCoord2f((float) m_srcWidth / (float) m_textureWidth, 0);
    glVertex2f(m_width, 0);

    //-- Bottom right
    glTexCoord2f((float) m_srcWidth / (float) m_textureWidth,
                 (float) m_srcHeight / (float) m_textureHeight);
    glVertex2f(m_width, m_height);

    //-- Bottom left
    glTexCoord2f(0, (float) m_srcHeight / (float) m_textureHeight);
    glVertex2f(0, m_height);

    glEnd();
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}

//----------------------------------------------

void TextureView::createTexture(unsigned int srcWidth, unsigned int srcHeight)
{
    m_srcWidth  = srcWidth;
    m_srcHeight = srcHeight;

    m_textureWidth  = ((m_srcWidth - 1) / 512 + 1) * 512;
    m_textureHeight = ((m_srcHeight - 1) / 512 + 1) * 512;;

    m_pTexture = new openni::RGB888Pixel[m_textureWidth * m_textureHeight];
}
