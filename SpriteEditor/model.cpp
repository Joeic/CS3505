// Code style reviewed by Nickolas Solum on 4/5/2023
#include "model.h"
#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>

///
/// \brief Model::Model constructor.
/// \param parent used by Qt
///
Model::Model(QWidget *parent)
    : QObject(parent),
      sprite(new Animation()),
      dialogParent(parent)
{
    paintSettings = Paint();
    currentTool = std::make_unique<Paintbrush>();

    connect(&sprite,
            &Animation::pushUndoState,
            this,
            &Model::pushUndoState);

    //Animates frames shown in animation preview. See playAnimation()
    connect(timer, &QTimer::timeout, [this]()
    {
        //change to next frame.
        sprite.changeFrameWhenAnimating(animationFramesIndex);

        //increment so can change to next frame. If at last frame, start from beginning
        if(animationFramesIndex < sprite.getSizeOfFramesVector()-1)
        {
            animationFramesIndex++;
        }
        else
        {
            animationFramesIndex = 0;
        }
        emit updateFramePicker(animationFramesIndex);
    });
}


///
/// \brief Model::brushSelectedState Select the brush tool
/// \return true
///
bool Model::brushSelectedState()
{
    currentTool = std::make_unique<Paintbrush>();
    return true;
}

///
/// \brief Model::onionSkinningSelectedState Toggle onion skinning
/// \return current state of onion skinning, true iff selected
///
bool Model::onionSkinningSelectedState()
{
    onionSkinningSelected = !onionSkinningSelected;
    sprite.getCurFrame()->afterCanvasChanged();
    return onionSkinningSelected;
}

///
/// \brief Model::dithererSelectedState Toggle dithering feature
/// \return current state of dithering, true iff selected
///
bool Model::dithererSelectedState()
{
    return paintSettings.switchDithering();
}

///
/// \brief Model::paintBucketSelectedState Select the paint bucket tool
/// \return true
///
bool Model::paintBucketSelectedState()
{
    currentTool = std::make_unique<PaintBucket>();
    return true;
}

///
/// \brief Model::eraserSelectedState Select the eraser tool
/// \return true
///
bool Model::eraserSelectedState()
{
    currentTool = std::make_unique<Eraser>();
    return true;
}

///
/// \brief Model::brushSizeValueChanged Set the brush size
/// \param value size to set the tool as.
///
void Model::brushSizeValueChanged(int size)
{
    paintSettings.setToolSize(size);
}

///
/// \brief Model::mouseClicked Slot that is called when the mouse is clicked on the frame
/// \param x - the x coordinate to draw the pixel on
/// \param y - the y coordinate to draw the pixel on
///
void Model::mouseClicked(int x, int y)
{
    currentTool->useToolAtPoint(sprite.getCurFrame(), paintSettings, x, y);
}

///
/// \brief Model::mouseMoeved Slot that is called when the mouse is moved on the frame
/// \param x - the most recent x coordinate to draw the pixel on
/// \param y - the most recent y coordinate to draw the pixel on
/// \param prevX - the last x coordinate to draw the pixel on
/// \param prevY - the last y coordinate to draw the pixel on
///
void Model::mouseMoved(int x, int y, int prevX, int prevY)
{
    currentTool->useToolOnLine(sprite.getCurFrame(), paintSettings, prevX, prevY, x, y);
}


///
/// \brief Model::getOnionSkinningSelected Return state of onion skinning selection
/// \return  true iff onion skinning is enabled
///
bool Model::getOnionSkinningSelected()
{
    return onionSkinningSelected;
}

///
/// \brief Model::save serialize to currently-open file, else open file picker then serialize
///
void Model::save()
{
    if(currentFile == tr(""))
        currentFile = pickSaveLocation("Save sprite...");
    if(currentFile == tr("")) // user cancelled
        return;

    writeToFile(currentFile);
}

///
/// \brief Model::saveAs open file picker and serialize to file
///
void Model::saveAs()
{
    QString newFile = pickSaveLocation("Save sprite as...");
    if(newFile == tr("")) // user cancelled
        return;
    currentFile = newFile;

    writeToFile(currentFile);
}

///
/// \brief Model::writeToFile helper method to serialize to a file
/// \param filename file to write to
/// \return true if written successfully
///
bool Model::writeToFile(QString filename)
{
    // We currently only support JSON, so use that.
    //   If we supported other formats, we would check the file extension here.

    QFile saveFile(filename);

    if(!saveFile.open(QIODevice::WriteOnly))
    {
        emit showWarning("Unable to write file", "Unable to open file for writing. Sprite is not saved.");
        return false;
    }

    QJsonObject ser = sprite.toJson();
    saveFile.write(QJsonDocument(ser).toJson(QJsonDocument::JsonFormat::Compact));

    return true;
}

///
/// \brief Model::load open file picker and deserialize from file
///
void Model::load()
{
    QString openFilename = QFileDialog::getOpenFileName(dialogParent, "Open sprite...", QString(), tr("JSON files (*.ssp)"));
    if(openFilename == tr("")) // user cancelled
        return;

    currentFile = openFilename;
    QFile openFile(openFilename);

    if(!openFile.open(QIODevice::ReadOnly))
    {
        emit showWarning("Unable to open file", "Unable to open file for reading. Cannot open sprite.");
        return;
    }

    QByteArray loadData = openFile.readAll();
    QJsonDocument loadDoc = QJsonDocument::fromJson(loadData);
    QJsonObject s = loadDoc.object();

    sprite.loadJson(s);
    purgeUndo();
}

///
/// \brief Model::pickSaveLocation helper method to open a file picker
/// \param caption title of the file picker window
/// \return selected file name or tr("") if cancelled
///
QString Model::pickSaveLocation(const QString& caption)
{
    return QFileDialog::getSaveFileName(dialogParent, caption, QString(), tr("JSON files (*.ssp)"));
}

///
/// \brief Model::addFrameToList add new frame to end of the list
///
void Model::addFrameToList()
{
    sprite.addFrame(sprite.getFrameSize());
}

///
/// \brief Model::deleteFrameFromList delete current frame
///
void Model::deleteFrameFromList()
{
    sprite.deleteFrame(sprite.getCurrFrameIndex());
}

///
/// \brief Model::frameClicked when click the frame in the QViewlist, set it to be current frame
/// \param index frame index selected
///
void Model::frameClicked(const QModelIndex &index)
{
    sprite.changeFrame(index.row());
}

///
/// \brief Model::pushUndoState after the user does something, save the change so it can be undone
/// \param s UndoState representing the user's change
///
void Model::pushUndoState(UndoState state)
{
    if(undoIndex < undoState.size())
    {
        // Remove all redo states
        undoState.resize(undoIndex, UndoState(UndoStateType::FRAME_ADD));
    }

    undoState.push_back(std::move(state));
    undoIndex = undoState.size();

    emit updateUndoDisabled(getUndoDisabled());
    emit updateRedoDisabled(getRedoDisabled());
}

///
/// \brief Model::purgeUndo throw away all undo data (e.g. for after a new file is loaded)
///
void Model::purgeUndo()
{
    undoState.clear();
    undoIndex = 0;

    emit updateUndoDisabled(getUndoDisabled());
    emit updateRedoDisabled(getRedoDisabled());
}

///
/// \brief Model::getUndoDisabled
/// \return true if the Undo menu option should be disabled
///
bool Model::getUndoDisabled()
{
    return undoIndex <= 0;
}

///
/// \brief Model::getRedoDisabled
/// \return true if the Redo menu option should be disabled
///
bool Model::getRedoDisabled()
{
    return undoIndex >= undoState.size();
}

///
/// \brief Model::undo slot to undo the most recent change
///
void Model::undo()
{
    if(undoIndex <= 0 || undoIndex > undoState.size())
        return;

    undoIndex--;

    // undo the change at undoIndex
    UndoState state = undoState[undoIndex];
    switch(state.type)
    {
        case UndoStateType::FRAME_CHANGE:
            sprite.replaceFrame(state.frame_start_index, std::make_shared<Frame>(*state.old_frame), false);
            break;
        case UndoStateType::FRAME_ADD:
            sprite.deleteFrame(state.frame_start_index, false);
            break;
        case UndoStateType::FRAME_DELETE:
            sprite.insertFrame(state.frame_start_index, std::make_shared<Frame>(*state.old_frame), false);
            break;
        case UndoStateType::FRAME_REINSERT:
            break;
    }
    emit updateUndoDisabled(getUndoDisabled());
    emit updateRedoDisabled(getRedoDisabled());
}

///
/// \brief Model::redo slot to redo the most recent change
///
void Model::redo()
{
    if(undoIndex >= undoState.size())
        return;

    // redo the change at undoIndex
    UndoState s = undoState[undoIndex];
    switch(undoState[undoIndex].type)
    {
        case UndoStateType::FRAME_CHANGE:
            sprite.replaceFrame(s.frame_start_index, std::make_shared<Frame>(*s.new_frame), false);
            break;
        case UndoStateType::FRAME_ADD:
            sprite.insertFrame(s.frame_start_index, std::make_shared<Frame>(*s.new_frame), false);
            break;
        case UndoStateType::FRAME_DELETE:
            sprite.deleteFrame(s.frame_start_index, false);
            break;
        case UndoStateType::FRAME_REINSERT:
            break;
    }

    undoIndex++;

    emit updateUndoDisabled(getUndoDisabled());
    emit updateRedoDisabled(getRedoDisabled());
}

///
/// \brief Model::copyFrame copy the current frame
///
void Model::copyFrame()
{
     int index = sprite.getCurrFrameIndex();
     sprite.copyFrame(index);
}

///
/// \brief Model::playAnimation helper for animation preview
/// \param fps frames per second
///
void Model::playAnimation(int fps)
{
    if(fps > 0)
    {
        //start at frame zero when new fps slider value is changed
        animationFramesIndex = 0;
        sprite.changeFrameWhenAnimating(animationFramesIndex);

        // Calculates interval between frames
        int interval = 1000 / fps;

        timer->setInterval(interval);

        timer->start();
    }
    else
    {
        //stop animation if fps is zero
        timer->stop();
    }
}


///
/// \brief Model::changeFrameShown Slider to change frame in animation preview
/// \param number of frame to show
///
void Model::changeFrameShown(int frame)
{
    if(sprite.getSizeOfFramesVector() >= 1)
    {
        emit numOfMadeFrames(sprite.getSizeOfFramesVector()-1);
        sprite.changeFrameWhenAnimating(frame);
    }
}

///
/// \brief Model::stopTimer Stop animation so that it doesn't affect pixel editor
///
void Model::stopTimer()
{
    timer->stop();
    //reset to shwoing frame 0 in animation preview when close window and open it back up.
    changeFrameShown(0);
}

///
/// \brief Model::setStartingAnimationFrame show the first frame
///
void Model::setStartingAnimationFrame()
{
    changeFrameShown(0);
}
