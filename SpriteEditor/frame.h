// Code style reviewed by Nickolas Solum on 4/5/2023
#ifndef FRAME_H
#define FRAME_H

#include <QImage>
#include <QJsonArray>
#include <QObject>

///
/// \brief The Frame class that contains the basic information of frame
/// \author Joey Cai, Kyle Holland
///
class Frame : public QObject
{
    Q_OBJECT

private:
    int frameWidth;
    int frameHeight;

public:
    enum class EditMode { Editable, ReadOnly };

    QImage old_canvas;
    QImage canvas;
    //QImage animationPreviewCanvas;
    Frame(int width, int height);
    Frame(int width, int height, const QJsonArray &fromJson);
    Frame(const Frame& other);
    Frame(const QImage& fromImg);

    //const QImage &getFrame() const;
    QJsonArray toJson();

    // call after any modification to canvas
    void afterCanvasChanged();

    int getFrameWidth();
    int getFrameHeight();
    void setFrameDimensions(int width, int height);

signals:
    void canvasChanged();
};

#endif // FRAME_H
