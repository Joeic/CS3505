// Code style reviewed by Kyle Holland on 4/5/2023
#ifndef ANIMATION_H
#define ANIMATION_H

#include "frame.h"
#include <list>
#include <memory>
#include <QAbstractListModel>
#include <QColor>
#include <QDataStream>
#include <QImage>
#include <QJsonObject>
#include <QMimeData>
#include <QPainter>
#include <QSize>
#include <QWidget>
#include "undostate.h"

///
/// \brief The Animation class constrols the all frames created in mainwindow.
/// It has a vector storing all frames, having operations such as add, delete
///  , copy, select, drag, save, load, set speed, and undo.
/// \author Joey Cai, Kyle Holland,
///
class Animation : public QAbstractListModel
{
    Q_OBJECT

    QSize frameSize;
    double playBackSpeed;

    int currFrameIndex;

    int currAnimationFrameIndex;

    std::vector<std::shared_ptr<Frame>> frames;
    std::vector<std::shared_ptr<Frame>> animationFrames;

    void linkCanvasChanged(std::shared_ptr<Frame> new_frame);

public:
    Animation(QObject *parent = nullptr, QSize frameSize = QSize(128, 128));
    void replaceFrame(int targetLocation, std::shared_ptr<Frame> frame, bool pushUndo = true);
    void insertFrame(int targetLocation, std::shared_ptr<Frame> frame, bool pushUndo = true);
    void addFrame(QSize, bool pushUndo = true);
    void setSpeed(double);
    void changeFrame(int);
    void deleteFrame(int targetLocation, bool pushUndo = true);
    void copyFrame(int targetLocation, bool pushUndo = true);
    void paintEvent(QPaintEvent *event) ;
    std::shared_ptr<Frame> getCurFrame();
    std::shared_ptr<Frame> getPrevFrame();
    QJsonObject toJson();
    void loadJson(const QJsonObject& fromJson);

    void changeFrameWhenAnimating(int frameIndex);

    int getCurrFrameIndex();

    // QAbstractListModel methods
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;
    void populate();
    void changeFrameDimensions(int width, int height);
    int getSizeOfFramesVector();
    int getFrameIndex();
    QSize getFrameSize();
    std::shared_ptr<Frame> getFrame(int index) const;

    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexList) const override;
    bool dropMimeData(const QMimeData *selectedFrame, Qt::DropAction drop, int rowInList, int columnInList, const QModelIndex &clickedIndex) override;
    Qt::DropActions supportedDropActions() const override;
    bool moveRows(const QModelIndex &start, int startRow, int rowsNum, const QModelIndex &end, int endRow) override;
    Qt::ItemFlags flags(const QModelIndex &selectedIndex) const override;

signals:
    void drawFrame (std::shared_ptr<Frame>);
    void disableDeleteButton(bool state);
    void pushUndoState(UndoState s);
    void drawAnimationFrame(std::shared_ptr<Frame>);
    void setStateofAnimationPreview(bool state);
};

Q_DECLARE_METATYPE(std::shared_ptr<Frame>)

#endif // ANIMATION_H
