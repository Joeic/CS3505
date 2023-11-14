//Code style reviewed by Cameron Wortmann 4/5/2023
#include "paintbucket.h"

///
/// \brief PaintBucket::useToolAtSinglePoint Uses paint bucket at the point with coordinates (x,y)
///        This flood fills frame from the point with the current color settings.
/// \param frame to use paint bucket on
/// \param paintSettings Settings for the tool, including color, dithering, and tool size.
/// \param x coordinate from which flood fill starts from
/// \param y coordinate from which flood fill starts from
///
void PaintBucket::useToolAtPoint(std::shared_ptr<Frame> frame, Paint paintSettings, int x, int y)
{
    int size = paintSettings.getToolSize();
    //For every width of the tool
    for (int i = 0; i < size && (x+i) < frame->canvas.width(); i++)
    {
        //For every height of the tool
        for (int j = 0; j < size && (y+j) < frame->canvas.height(); j++)
        {
            //Add starting point to queue of pixels to paint.
            QColor oldColor = frame->canvas.pixelColor(x + i, y + j);       
            std::queue <QPoint> validPixelQueue;
            validPixelQueue.push(QPoint (x + i, y + j));

            //While there are pixels that should be painted
            while (!validPixelQueue.empty())
            {
                //Take out next paint that should be painted
                QPoint currentPoint = validPixelQueue.front();
                validPixelQueue.pop();
                int currentX = currentPoint.x();
                int currentY = currentPoint.y();

                //If it should still be painted (may have already been painted by other path)
                if (validPaintablePoint(frame, paintSettings, oldColor, currentX, currentY))
                {
                    //Paint pixel and add neighboring pixels that should be painted to the queue
                    frame->canvas.setPixelColor(currentX, currentY, paintSettings.getColorAtCoordi(currentX,currentY));
                    if (validPaintablePoint(frame, paintSettings, oldColor, currentX + 1, currentY))
                    {
                        validPixelQueue.push(QPoint (currentX + 1, currentY));
                    }
                    if (validPaintablePoint(frame, paintSettings, oldColor, currentX - 1, currentY))
                    {
                        validPixelQueue.push(QPoint (currentX - 1, currentY));
                    }
                    if (validPaintablePoint(frame, paintSettings, oldColor, currentX, currentY + 1))
                    {
                        validPixelQueue.push(QPoint (currentX, currentY + 1));
                    }
                    if (validPaintablePoint(frame, paintSettings, oldColor, currentX, currentY - 1))
                    {
                        validPixelQueue.push(QPoint (currentX, currentY - 1));
                    }
                }
            }
        }
    }

    frame->afterCanvasChanged();
}

///
/// \brief PaintBucket::useToolOnLine Uses paintbucket at all points between points with coordinates (x1,y1) and (x2.y2)
///        This colors the pixels within the tool to the colors specified in paintSettings, dithering if option is on.
/// \param frame to use paintbucket on
/// \param paintSettings Settings for the tool, including color, dithering, and tool size.
/// \param x1 x coordinate of one point of the line
/// \param y1 y coordinate of one point of the line
/// \param x2 x coordinate of other point of the line
/// \param y2 y coordinate of other point of the line.
///
void PaintBucket::useToolOnLine(std::shared_ptr<Frame> frame, Paint paintSettings, int x1, int y1, int x2, int y2)
{
    QImage stencil = lineStencil(frame->canvas, paintSettings.getToolSize(), x1, y1, x2, y2);
    //For every x value on the stencil
    for (int currentX = 0; currentX < stencil.size().width(); currentX++)
    {
        //For every y value on the stencil
        for (int currentY = 0; currentY < stencil.size().height(); currentY++)
        {
            //If stencil filled in for this pixel, use paint bucket starting on this point.
            if (stencil.pixelColor(currentX, currentY) == Qt::black)
            {
                useToolAtPoint(frame, paintSettings, currentX, currentY);
            }
        }
    }
}

///
/// \brief PaintBucket::validPaintablePoint Private method used to determine if a point should be painted with flood fill algorithm.
/// \param frame to use paint bucket on
/// \param newColor Paint to get color to replace old color with
/// \param oldColor color that is being replaced with flood fill algorithm
/// \param x coordinate from which flood fill starts from
/// \param y coordinate from which flood fill starts from
/// \return True iff the point given with (x,y) should be painted based on flood fill algorithm
///
bool PaintBucket::validPaintablePoint(std::shared_ptr<Frame> frame, Paint newColor, QColor oldColor, int x, int y)
{
    if(frame->canvas.valid(x, y)) //If (x,y) is a point within the canvas
    {
        if (frame->canvas.pixelColor(x, y) != newColor.getColorAtCoordi(x,y))
        {
            if (frame->canvas.pixelColor(x, y) == oldColor)
            {
                return true;
            }
        }
    }
    return false;
}
