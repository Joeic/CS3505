// Code style reviewed by Kyle Holland on 4/5/2023
#include "eraser.h"

///
/// \brief Eraser::useToolAtPoint Uses eraser at point with coordinates (x,y) This erases the
///        pixels within the tool.
/// \param frame to use eraser on
/// \param paintSettings Settings for the tool. Only use toolSize for eraser
/// \param x coordinate of base of eraser
/// \param y coordinate of base of eraser
///
void Eraser::useToolAtPoint(std::shared_ptr<Frame> frame, Paint paintSettings, int x, int y)
{
    int size = paintSettings.getToolSize();
    //For every width of the tool
    for (int i = 0; i < size && (x+i) < frame->canvas.width(); i++)
    {
        //For every height of the tool
        for (int j = 0; j < size && (y+j) < frame->canvas.height(); j++)
        {
            frame->canvas.setPixelColor(x + i, y + j, Qt::transparent);
        }
    }

    frame->afterCanvasChanged();
}


///
/// \brief Eraser::useToolOnLine Uses eraser at all points between points with coordinates (x1,y1) and (x2.y2)
///        This erases then pixels within the tool, with size of line dependent on paintSettings.
/// \param frame to use paintbrush on
/// \param paintSettings Settings for the tool. Use toolSize for eraser.
/// \param x1 x coordinate of one point of the line
/// \param y1 y coordinate of one point of the line
/// \param x2 x coordinate of other point of the line
/// \param y2 y coordinate of other point of the line.
///
void Eraser::useToolOnLine(std::shared_ptr<Frame> frame, Paint paintSettings, int x1, int y1, int x2, int y2)
{
    QImage stencil = lineStencil(frame->canvas, paintSettings.getToolSize(), x1, y1, x2, y2);
    //For every x value on the stencil
    for (int currentX = 0; currentX < stencil.size().width(); currentX++)
    {
        //For every y value on the stencil
        for (int currentY = 0; currentY < stencil.size().height(); currentY++)
        {
            //If stencil filled in, erase pixel
            if (stencil.pixelColor(currentX, currentY) == Qt::black)
            {
               frame->canvas.setPixelColor(currentX, currentY, Qt::transparent);
            }
        }
    }

    frame->afterCanvasChanged();
}

