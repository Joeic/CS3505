//Code style reviewed by Cameron Wortmann 4/5/2023
#include "tool.h"

///
/// \brief Tool::lineStencil Returns a qimage of a black line with lineSize thickness on a
///        white canvas between two given points. To be used as a stencil.
/// \param canvas QImage that the line is going to be drawn on
/// \param lineSize the size of the line that will be drawn between the two points
/// \param x1 x coordinate of the first point
/// \param y1 y coordinate of the first point
/// \param x2 x coordinate of the second point
/// \param y2 y coordinate of the second point.
/// \return QImage stencil with black line drawn on white canvas between two given points.
///
QImage Tool::lineStencil(QImage canvas, int lineSize, int x1, int y1, int x2, int y2)
{
    QImage stencil = QImage(canvas.size(), QImage::Format_RGB32);
    stencil.fill(Qt::white);
    QPainter painter;
    painter.begin(&stencil);
    painter.setPen(Qt::black);
    for (int i = 0; i < lineSize; i++)
    {
        for (int j = 0; j < lineSize; j++)
        {
            painter.drawLine(x1 + i, y1 + j, x2 + i, y2 + j);
        }
    }
    painter.end();
    return stencil;
}
