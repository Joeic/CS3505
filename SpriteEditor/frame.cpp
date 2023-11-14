// Code style reviewed by Nickolas Solum on 4/5/2023
#include "frame.h"
#include <QPainter>
#include <QPalette>

///
/// \brief Frame::Frame frame constructor, setting up the size and background color
/// \param width
/// \param height
///
Frame::Frame(int width, int height)
    : frameWidth(width),
      frameHeight(height),
      canvas(width, height, QImage::Format_ARGB32)
{
    QColor frameColor(Qt::transparent);
    canvas.fill(frameColor);
    afterCanvasChanged();
}

///
/// \brief Frame::Frame deserialize a Frame from JSON
/// \param width width in pixels of the Frame's canvas
/// \param height height in pixels of the Frame's canvas
/// \param fromJson nested JSON arrays holding the Frame's pixel data
///
Frame::Frame(int width, int height, const QJsonArray &fromJson)
    : frameWidth(width),
      frameHeight(height),
      canvas(width, height, QImage::Format_ARGB32)
{
    QJsonArray::const_iterator row = fromJson.cbegin();
    for(int y = 0; y < height && row != fromJson.cend(); y++)
    {
        QJsonArray rowArr = row->toArray();
        QJsonArray::const_iterator col = rowArr.cbegin();
        for(int x = 0; x < canvas.height() && col != rowArr.cend(); x++)
        {
            QJsonArray pixel = col->toArray();
            QColor px(pixel[0].toInt(), pixel[1].toInt(), pixel[2].toInt(), pixel[3].toInt());
            canvas.setPixelColor(x, y, px);

            col++;
        }
        row++;
    }
}

///
/// \brief Frame::Frame copy constructor for Frame. Copies just the frame data; QObject signals/slots will not be copied.
/// \param other Frame to copy
///
Frame::Frame(const Frame& other)
    : QObject(nullptr)
{
    frameWidth = other.frameWidth;
    frameHeight = other.frameHeight;
    canvas = other.canvas;
}

///
/// \brief Frame::Frame make a Frame from a QImage
/// \param fromImg QImage to copy into this Frame
///
Frame::Frame(const QImage& fromImg)
    : QObject(nullptr)
{
    frameWidth = fromImg.width();
    frameHeight = fromImg.height();
    canvas = fromImg.copy();
}

///
/// \brief Frame::toJson serialize this Frame's pixel data to a nested JSON array
/// \return nested JSON array
///
QJsonArray Frame::toJson()
{
    QJsonArray rows;
    for(int y = 0; y < canvas.height(); y++)
    {
        QJsonArray row;
        for(int x = 0; x < canvas.height(); x++)
        {
            QRgb px = canvas.pixel(x, y);
            QJsonArray pixel{qRed(px), qGreen(px), qBlue(px), qAlpha(px)};
            row.append(std::move(pixel));
        }
        rows.append(std::move(row));
    }
    return rows;
}

///
/// \brief Frame::afterCanvasChanged call after modifying this Frame's canvas
///
void Frame::afterCanvasChanged()
{
    emit canvasChanged(); // slots will see canvas with the new state and old_canvas with the old state
    old_canvas = canvas.copy();
}

///
/// \brief Frame::getFrameWidth Gets the frame's width
/// \return The frame's width
///
int Frame::getFrameWidth()
{
    return frameWidth;
}

///
/// \brief Frame::getFrameHeight Gets the frame's height
/// \return The frame's height
///
int Frame::getFrameHeight()
{
    return frameHeight;
}

///
/// \brief Frame::setFrameDimensions Changes the frames dimensions
/// \param newWidth - the desired width of the frame
/// \param newHeight - the desired height of the frame
///
void Frame::setFrameDimensions(int newWidth, int newHeight)
{
    frameWidth = newWidth;
    frameHeight = newHeight;
    canvas = canvas.scaled(QSize(frameWidth, frameHeight));
    afterCanvasChanged();
}
