//Code style reviewed by Cameron Wortmann on 4/5/2023
#include "paintbrush.h"

///
/// \brief Paintbrush::useToolAtPoint Uses paintbrush at point with coordinates (x,y) This colors the
///        pixels within the tool to the colors specified in paintSettings, dithering if option is on.
/// \param frame to use paintbrush on
/// \param paintSettings Settings for the tool, including color, dithering, and tool size.
/// \param x coordinate of base of paintbrush
/// \param y coordinate of base of paintbrush
///
void Paintbrush::useToolAtPoint(std::shared_ptr<Frame> frame, Paint paintSettings, int x, int y)
{
    int size = paintSettings.getToolSize();
    //For every width of the tool
    for (int i = 0; i < size && (x+i) < frame->canvas.width(); i++)
    {
        //For every height of the tool
        for (int j = 0; j < size && (y+j) < frame->canvas.height(); j++)
        {
            frame->canvas.setPixelColor(x + i, y + j, paintSettings.getColorAtCoordi(x + i,y + j));
        }
    }

    frame->afterCanvasChanged();
}

///
/// \brief Paintbrush::useToolOnLine Uses paintbrush at all points between points with coordinates (x1,y1) and (x2.y2)
///        This colors then pixels within the tool to the colors specified in paintSettings, dithering if option is on.
/// \param frame to use paintbrush on
/// \param paintSettings Settings for the tool, including color, dithering, and tool size.
/// \param x1 x coordinate of one point of the line
/// \param y1 y coordinate of one point of the line
/// \param x2 x coordinate of other point of the line
/// \param y2 y coordinate of other point of the line.
///
void Paintbrush::useToolOnLine(std::shared_ptr<Frame> frame, Paint paintSettings, int x1, int y1, int x2, int y2)
{
    QImage stencil = lineStencil(frame->canvas, paintSettings.getToolSize(), x1, y1, x2, y2);
    //For every x value on the stencil
    for (int currentX = 0; currentX < stencil.size().width(); currentX++)
    {
        //For every y value on the stencil
        for (int currentY = 0; currentY < stencil.size().height(); currentY++)
        {
            //If stencil filled in, color pixel accordingly
            if (stencil.pixelColor(currentX, currentY) == Qt::black)
            {
               frame->canvas.setPixelColor(currentX, currentY, paintSettings.getColorAtCoordi(currentX,currentY));
            }
        }
    }

    frame->afterCanvasChanged();
}
