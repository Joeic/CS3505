#ifndef PAINTBUCKET_H
#define PAINTBUCKET_H

#include "tool.h"
#include <queue>

///
/// \brief The Paintbucket class a tool that can be used on the canvas to fill areas of pixels (set color).
///        Color and size of fill, as well as dithering, set in Paint class passed to methods.
/// \author Nickolas Solum
/// Code style reviewed by Cameron Wortmann on 4/5/2023
///
class PaintBucket : public Tool
{
    bool validPaintablePoint(std::shared_ptr<Frame> frame, Paint newColor, QColor oldColor, int x, int y);

public:
    void useToolAtPoint(std::shared_ptr<Frame> frame, Paint color, int x, int y);
    void useToolOnLine(std::shared_ptr<Frame> frame, Paint color, int x1, int y1, int x2, int y2);
};

#endif // PAINTBUCKET_H
