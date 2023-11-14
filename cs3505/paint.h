#ifndef PAINT_H
#define PAINT_H

#include <QColor>

///
/// \brief The Paint class stores a set of drawing settings (colors and styles)
/// \author Nickolas Solum
///
class Paint
{
private:
    QColor primaryColor;
    QColor secondaryColor;
    bool dithering;
    int toolSize;
public:
    Paint();
    QColor getColorAtCoordi(int x, int y);

    int getToolSize();
    void setToolSize(int size);

    bool switchDithering();

    QColor getPrimaryColor();
    QColor getSecondaryColor();

    void setPrimaryColor(const QColor &primary);
    void setSecondaryColor(const QColor &secondary);
};

#endif // PAINT_H
