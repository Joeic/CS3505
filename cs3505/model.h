// Code style reviewed by Nickolas Solum on 4/5/2023
#ifndef MODEL_H
#define MODEL_H

#include "animation.h"
#include "eraser.h"
#include "frame.h"
#include <memory>
#include <optional>
#include "paint.h"
#include "paintbrush.h"
#include "paintbucket.h"
#include "tool.h"
#include "undostate.h"
#include <QJsonObject>
#include <QMouseEvent>
#include <QString>
#include <QTimer>
#include <QWidget>

///
/// \brief The Model class hold the state of the sprite editor and contains much of the code to manipulate that state.
/// \author Ayden Smith, Kyle Holland, Joey Cai, Cameron Wortmann
///
class Model : public QObject
{
    Q_OBJECT

    std::unique_ptr<Tool> currentTool;

    QColor getPrimaryColor();
    QColor getSecondaryColor();
    bool onionSkinningSelected = false;
    bool dithererSelected = false;

    std::vector<UndoState> undoState;
    size_t undoIndex = 0;

    void purgeUndo();

    int animationFramesIndex = 0;

public:
    Model(QWidget *parent = nullptr);

    Paint paintSettings;
    Animation sprite;

    //Animation spriteAnimation;

    QWidget *dialogParent;

    bool getUndoDisabled();
    bool getRedoDisabled();
    bool getOnionSkinningSelected();

public slots:
    bool brushSelectedState();
    bool onionSkinningSelectedState();
    bool dithererSelectedState();
    bool paintBucketSelectedState();
    bool eraserSelectedState();

    void mouseClicked(int x, int y);
    void mouseMoved(int x, int y, int prevX, int prevY);
    void brushSizeValueChanged(int value);

    void addFrameToList();
    void deleteFrameFromList();;
    void frameClicked(const QModelIndex &index);
    void copyFrame();

    void save();
    void saveAs();
    void load();

    void undo();
    void redo();

    void changeFrameShown(int frame);
    void playAnimation(int fps);
    void stopTimer();
    void setStartingAnimationFrame();

private slots:
    void pushUndoState(UndoState s);

signals:
    void showWarning(const QString& title, const QString& text);

    void updateUndoDisabled(bool disabled);
    void updateRedoDisabled(bool disabled);

    void numOfMadeFrames(int frames);
    void updateFramePicker(int currFrame);


private:
    // "Save" will write to this file, if it's set
    QString currentFile;

    // Used by both "Save" and "Save As" -- configure and show file dialog
    QString pickSaveLocation(const QString& caption);

    bool writeToFile(QString filename);

   // QTimer *timer;

    QTimer* timer = new QTimer(this);


};

#endif // MODEL_H
