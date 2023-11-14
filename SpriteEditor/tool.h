#ifndef TOOL_H
#define TOOL_H

#include "frame.h"
#include <memory>
#include "paint.h"
#include <QPainter>

///
/// \brief The tool class defines methods that tools must implement.
/// \author Nickolas Solum
/// code style reviewed by Cameron Wortmann 4/5/2023
///
class Tool
{
protected:
    QImage lineStencil(QImage canvas, int toolSize, int x1, int y1, int x2, int y2);
public:
    virtual void useToolAtPoint(std::shared_ptr<Frame> frame, Paint color, int x, int y) = 0;
    virtual void useToolOnLine(std::shared_ptr<Frame> frame, Paint color, int x1, int y1, int x2, int y2) = 0;
};

#endif // TOOL_H
