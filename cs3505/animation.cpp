// Code style reviewed by Kyle Holland on 4/5/2023
#include "animation.h"
#include "undostate.h"
#include <QDataStream>
#include <QDebug>
#include <QIODevice>
#include <QMimeData>

///
/// \brief Animation::Animation constructor of Animation, set up the frame size and initialize to one frame
/// \param parent QObject
/// \param frameSize size of the frame
///
Animation::Animation(QObject *parent, QSize frameSize)
    : QAbstractListModel(parent),
      frameSize(frameSize),
      playBackSpeed(1.0)
{
    addFrame(frameSize);
}

///
/// \brief Animation::replaceFrame
/// \param targetLocation
/// \param frame
/// \param pushUndo
///
void Animation::replaceFrame(int targetLocation, std::shared_ptr<Frame> frame, bool pushUndo)
{
    beginResetModel();
    std::shared_ptr old_frame = frames[targetLocation];
    frames[targetLocation] = frame;
    endResetModel();
    drawFrame(frames[targetLocation]);

    linkCanvasChanged(frame);
    if(pushUndo)
        emit pushUndoState(UndoState::forFrameChange(targetLocation, old_frame, std::make_shared<Frame>(*frame)));
}

///
/// \brief Animation::insertFrame
/// \param targetLocation
/// \param frame
/// \param pushUndo
///
void Animation::insertFrame(int targetLocation, std::shared_ptr<Frame> frame, bool pushUndo)
{
    beginResetModel();
    frames.insert(frames.begin() + targetLocation, frame);
    endResetModel();
     drawFrame(frames[targetLocation]);

    linkCanvasChanged(frame);
    if(pushUndo)
        emit pushUndoState(UndoState::forFrameAdd(targetLocation, std::make_shared<Frame>(*frame)));
}

///
/// \brief Animation::addFrame add new frame to the list and display it in mainwindow
/// \param frameSize size of added frame
/// \param pushUndo determine if there is undo request
///
void Animation:: addFrame(QSize frameSize, bool pushUndo)
{
    int height = frameSize.height();
    int width = frameSize.width();
    //create a new frame that depends on the provided size
    std::shared_ptr<Frame> addedFrame = std::make_shared<Frame>(width, height);
    beginResetModel();
    frames.push_back(addedFrame);
    endResetModel();
    //add new frame to the tail of list
    currFrameIndex = frames.size() - 1;
    //automatically display just added frame on mainwindow
    emit drawFrame(frames[currFrameIndex]);
    emit disableDeleteButton(false);
    emit setStateofAnimationPreview(false);

    linkCanvasChanged(addedFrame);
    if(pushUndo)
        emit pushUndoState(UndoState::forFrameAdd(currFrameIndex, std::make_shared<Frame>(*addedFrame)));
}

///
/// \brief Animation::setSpeed set the Animation preview speed
/// \param speed Animation speed
///
void Animation::setSpeed(double speed)
{
    playBackSpeed = speed;
}

///
/// \brief Animation::changeFrame select target frame
/// \param targetLocation target location of selected frame
///
void Animation::changeFrame(int targetLocation)
{
    currFrameIndex = targetLocation;
    emit drawFrame(frames[targetLocation]);
}

///
/// \brief Animation::changeFrameDimensions Changes every frames dimensions
/// \param width - the desired width of every frame
/// \param height - the desired height of every frame
///
void Animation::changeFrameDimensions(int width, int height)
{
    frameSize = QSize(width, height);
    for(std::shared_ptr<Frame> frame : frames)
    {
        frame->setFrameDimensions(width, height);
    }
}

void Animation::changeFrameWhenAnimating(int frameIndex)
{   currFrameIndex = frameIndex;
    emit drawAnimationFrame(frames[frameIndex]);
}

///
/// \brief Animation::deleteFrame delete selected frame
/// \param targetLocation selected index of frame
/// \param pushUndo determine if there is undo request
///
void Animation::deleteFrame(int targetLocation, bool pushUndo)
{
    //save selected frame to be deleted
    std::shared_ptr<Frame> to_delete = frames[targetLocation];
    //if the index of selected frame is in a proper range of list's size while list has one than one frame, then delete selected frame
    if(targetLocation >= 0 && targetLocation < (int)frames.size() && frames.size() > 1)
    {
        beginResetModel();
        frames.erase(frames.begin() + targetLocation);
        endResetModel();
    }
    //if the selected frame is the only frame in the list, then disable delete button
    if(currFrameIndex >= (int)frames.size())
    {
        if(frames.size() == 1)
        {
            emit disableDeleteButton(true);
            emit setStateofAnimationPreview(true);
        }

            currFrameIndex = (int)frames.size() - 1;
    }
    emit drawFrame(frames[currFrameIndex]);

    if(pushUndo)
        emit pushUndoState(UndoState::forFrameDelete(targetLocation, to_delete));
}

///
/// \brief Animation::copyFrame copy the selected frame
/// \param targetLocation selected index of frame
/// \param pushUndo determine if there is undo request
///
void Animation::copyFrame(int targetLocation, bool pushUndo)
{
    beginResetModel();
    currFrameIndex = targetLocation + 1;
    std::shared_ptr<Frame> newFrame = std::make_shared<Frame>(*frames[targetLocation]);
    //create copied frame behind selected frame
    frames.insert(frames.begin() + targetLocation + 1, newFrame);
    endResetModel();
    //make mainwindow display the just copied frame
    emit drawFrame(frames[currFrameIndex]);
    emit disableDeleteButton(false);
    emit setStateofAnimationPreview(false);

    linkCanvasChanged(newFrame);
    if(pushUndo)
        emit pushUndoState(UndoState::forFrameAdd(targetLocation + 1, std::make_shared<Frame>(*newFrame)));
}

///
/// \brief Animation::getCurFrame current frame getter
/// \return current frame
///
std::shared_ptr<Frame> Animation::getCurFrame()
{
    return frames[currFrameIndex];
}

///
/// \brief Animation::getPrevFrame previous frame getter
/// \return previous frame
///
std::shared_ptr<Frame> Animation::getPrevFrame()
{
    if (currFrameIndex >= 1)
        return frames[currFrameIndex-1];
    else
        return frames[currFrameIndex];
}

///
/// \brief Animation::paintEvent event to be called to draw
/// \param event event to be invoked
///
void Animation::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}

///
/// \brief Animation::toJson serialize the Animation to a Qt JSON object
/// \return a QJsonObject containing keys height (int), width (int), numberOfFrames (int), and frames (object) [see Frame::toJson]
///
QJsonObject Animation::toJson()
{
    QJsonObject s;
    s["height"] = frameSize.height();
    s["width"] = frameSize.width();
    s["numberOfFrames"] = (int)frames.size();

    QJsonObject s_frames;
    size_t frame_n = 0;
    for(std::shared_ptr<Frame> f : frames)
    {
        s_frames[QString::fromStdString("frame" + std::to_string(frame_n))] = std::move(f->toJson());
        frame_n++;
    }
    s["frames"] = std::move(s_frames);

    return s;
}

///
/// \brief Animation::loadJson replace this Animation's data with that deserialized from the provided Qt JSON object
/// \param fromJson Qt JSON object with keys height (int), width (int), numberOfFrames (int), and frames (object) [see Frame's constructor that takes JSON]
///
void Animation::loadJson(const QJsonObject& fromJson)
{
    int frameCount = fromJson["numberOfFrames"].toInt();
    if(frameCount < 1)
    {
        // TODO: display warning
        return;
    }

    beginResetModel();

    frames.clear();
    frameSize.setWidth(fromJson["width"].toInt());
    frameSize.setHeight(fromJson["height"].toInt());

    QJsonObject jsonFrames = fromJson["frames"].toObject();
    for(int i = 0; i < frameCount; i++)
    {
        QString frameName = QString::fromStdString("frame" + std::to_string(i));
        QJsonArray f = jsonFrames[frameName].toArray();
        std::shared_ptr addedFrame = std::make_shared<Frame>(frameSize.width(), frameSize.height(), f);
        frames.push_back(addedFrame);
        linkCanvasChanged(addedFrame);
    }

    if(frameCount > 0)
    {
        emit disableDeleteButton(false);
    }

    endResetModel();

    changeFrame(0);
}

///
/// \brief Animation::rowCount used for QListView; gives the number of "rows" in the tabular data to be displayed
/// \param parent unused
/// \return the number of frames
///
int Animation::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return frames.size();
}

///
/// \brief Animation::columnCount used for QListView; gives the number of "columns" in the tabular data to be displayed
/// \param parent unused
/// \return 1
///
int Animation::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return 1;
}

///
/// \brief Animation::data get a "cell" value in the tabular data required by QListView
/// \param index "row" and "column" to get data from (only column 0 is valid)
/// \param role should be Qt::DisplayRole
/// \return a QVariant containing an integer referring to the frame to be displayed
///
QVariant Animation::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole)
    {
        if(index.column() == 0)
            return QVariant::fromValue(index.row());
    }
    return QVariant();
}

///
/// \brief Animation::getCurrFrameIndex current frame index getter
/// \return the current frame index
///
int Animation::getCurrFrameIndex()
{
    return currFrameIndex;
}

///
/// \brief Animation::linkCanvasChanged connect the canvasChanged signal on the given frame to code that tracks undo states
/// \param new_frame the Frame whose signal to connect to
///
void Animation::linkCanvasChanged(std::shared_ptr<Frame> new_frame)
{
    connect(new_frame.get(),
            &Frame::canvasChanged,
            [this, new_frame]
            {
                if(new_frame->old_canvas == new_frame->canvas)
                    return;

                int idx = std::distance(frames.cbegin(), std::find(frames.cbegin(), frames.cend(), new_frame));
                emit pushUndoState(UndoState::forFrameChange(idx, std::make_shared<Frame>(new_frame->old_canvas), std::make_shared<Frame>(new_frame->canvas)));
            });
}

///
/// \brief Animation::getSizeOfFramesVector get the number of frames
/// \return the number of frames
///
int Animation::getSizeOfFramesVector()
{
    return frames.size();
}

///
/// \brief Animation::getFrameSize size of frame getter
/// \return QSize type of frame size
///
QSize Animation:: getFrameSize()
{
    return frameSize;
}

///
/// \brief Animation::moveRows used by drag operation
/// \param start from start locaiton
/// \param startRow row index of start
/// \param rowsNum count of rows
/// \param end to end location
/// \param endRow end index
/// \return
///
bool Animation::moveRows(const QModelIndex &start, int startRow, int rowsNum, const QModelIndex &end, int endRow)
{
    //if try to drag somewhere outside, refuse to drag
    if (startRow < 0 || endRow < 0 || startRow >= rowCount() || endRow >= rowCount())
    {
        return false;
    }

    //change the current frame to other location
    beginMoveRows(start, startRow, startRow + rowsNum - 1, end, endRow > startRow ? endRow + 1 : endRow);
    std::shared_ptr<Frame> movingFrame = frames[startRow];
    frames.erase(frames.begin() + startRow);
    frames.insert(frames.begin() + endRow, movingFrame);
    endMoveRows();
    currFrameIndex = endRow;
    emit drawFrame(frames[currFrameIndex]);
    return true;
}

///
/// \brief Animation::flags set the items in the list can be drag and drop
/// \param selectedIndex selected frame
/// \return flag
///
Qt::ItemFlags Animation::flags(const QModelIndex &selectedIndex) const
{
    return Qt::ItemIsDragEnabled | Qt::ItemIsSelectable| Qt::ItemIsEnabled  | Qt::ItemIsDropEnabled;
}

///
/// \brief Animation::getFrame get specific frame
/// \param index the frame selected
/// \return selected frame
///
std::shared_ptr<Frame> Animation::getFrame(int index) const
{
    return frames[index];
}

///
/// \brief Animation::mimeTypes serve for drag
/// \return  the typr of animation.frame
///
QStringList Animation::mimeTypes() const
{
    QStringList mimeTypes;
    mimeTypes << "application/vnd.animation.frame";
    return mimeTypes;
}

///
/// \brief Animation::supportedDropActions serve for drag so that frame can be moved
/// \return moveaction
///
Qt::DropActions Animation::supportedDropActions() const
{
    return Qt::MoveAction;
}

///
/// \brief Animation::dropMimeData operation of drop
/// \param selectedFrame
/// \param drop
/// \param rowInList
/// \param columnInList
/// \param clickedIndex
/// \return  determine if it's okay to drag
///
bool Animation::dropMimeData(const QMimeData *selectedFrame, Qt::DropAction drop, int rowInList, int columnInList, const QModelIndex &clickedIndex)
{
    if (drop == Qt::IgnoreAction)
        return true;

    if (!selectedFrame->hasFormat("application/vnd.animation.frame") || columnInList > 0)
        return false;

    int currRow = rowInList;
    if (rowInList == -1)
    {
        //get target location
        if(clickedIndex.isValid())
        {
               currRow = clickedIndex.row();
        }
        else
        {
               currRow = rowCount();
        }
    }
    int sourceRow;
    QByteArray selectFrame = selectedFrame->data("application/vnd.animation.frame");
    QDataStream stream(&selectFrame, QIODevice::ReadOnly);
    stream >> sourceRow;

    //invode moveRows method to move

    if (sourceRow != currRow)
    {
        moveRows(index(sourceRow, 0), sourceRow, 1, QModelIndex(), currRow);
    }

    return true;
}

///
/// \brief Animation::mimeData get the mimedata of drag
/// \param indexList
/// \return
///
QMimeData *Animation::mimeData(const QModelIndexList &indexList) const
{
    QByteArray selectFrame;
    QMimeData *setFrame = new QMimeData();
    QDataStream stream(&selectFrame, QIODevice::WriteOnly);
    foreach (const QModelIndex &index, indexList)
    {
        stream << index.row();
    }
    setFrame->setData("application/vnd.animation.frame", selectFrame);
    return setFrame;
}
