// Code style reviewed by Cameron Wortmann on 4/5/2023
#include "paint.h"

Paint::Paint()
{
    primaryColor = QColor(0, 0, 0, 255);
    secondaryColor =  QColor(255, 255, 255, 255);
    dithering = false;
    toolSize = 1;
}

///
/// \brief Paint::getColorAtCoordi Returns the color that a pixel should be painted to.
///        based on primary color, secondary color, and dithering.
/// \param x X coordinate of the pixel
/// \param y Y coordinate of the pixel
/// \return QColor color that the pixel should be.
///
QColor Paint::getColorAtCoordi(int x, int y)
{
    if (dithering)
    {
        if ((x%2) == 0)
        {
            if ((y%2) == 0)
            {
                return primaryColor;
            }
            else
            {
                return secondaryColor;
            }
        }
        else
        {
            if ((y%2) == 0)
            {
                return secondaryColor;
            }
            else
            {
                return primaryColor;
            }
        }
    }
    else
    {
        return primaryColor;
    }
}

///
/// \brief Paint::getToolSize Returns the size of the tool
/// \return int toolSize
///
int Paint::getToolSize()
{
    return toolSize;
}

///
/// \brief Paint::setToolSize Sets the size of tool
/// \param size toolSize
///
void Paint::setToolSize(int size)
{
    toolSize = size;
}

bool Paint::switchDithering()
{
    dithering = !dithering;
    return dithering;
}

QColor Paint::getPrimaryColor()
{
    return primaryColor;

}

QColor Paint::getSecondaryColor()
{
    return secondaryColor;
}

void Paint::setPrimaryColor(const QColor &primary)
{
    primaryColor = primary;
}
void Paint::setSecondaryColor(const QColor &secondary)
{
    secondaryColor = secondary;
}
