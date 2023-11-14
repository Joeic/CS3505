// Code style reviewed by Kyle Holland on 4/5/2023
#ifndef ERASER_H
#define ERASER_H

#include "tool.h"

///
/// \brief The Eraser class a tool that can be used on the canvas to erase pixels (set as invisible).
/// \author Nickolas Solum
///
class Eraser : public Tool
{
public:
    void useToolAtPoint(std::shared_ptr<Frame> frame, Paint color, int x, int y);
    void useToolOnLine(std::shared_ptr<Frame> frame, Paint color, int x1, int y1, int x2, int y2);
};

#endif // ERASER_H
