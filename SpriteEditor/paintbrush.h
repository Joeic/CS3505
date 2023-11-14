#ifndef PAINTBRUSH_H
#define PAINTBRUSH_H

#include "tool.h"

///
/// \brief The Paintbrush class a tool that can be used on the canvas to draw pixels (set color).
///        Color and size of paintbrush, as well as dithering, set in Paint class passed to methods.
/// \author Nickolas Solum
/// Code style reviewed by Cameron Wortmann on 4/5/2023
///
class Paintbrush : public Tool
{
public:
    void useToolAtPoint(std::shared_ptr<Frame> frame, Paint color, int x, int y);
    void useToolOnLine(std::shared_ptr<Frame> frame, Paint color, int x1, int y1, int x2, int y2);
};

#endif // PAINTBRUSH_H
