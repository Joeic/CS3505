// Code style reviewed by Kyle Holland on 4/5/2023
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "frameitemdelegate.h"
#include <QColorDialog>
#include <QMessageBox>

///
/// \brief MainWindow::MainWindow root window for the sprite editor
/// \param _model reference to the Model containing editor state and supporting code
/// \param parent used by Qt
///
MainWindow::MainWindow(std::shared_ptr<Model> _model, QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      model(_model),
      animationPreview(_model)
{
    ui->setupUi(this);
    model->dialogParent = this;
    QTimer::singleShot(0, this, &MainWindow::drawCurrentFrame);

    ui->brushSelector->setCheckable(true);
    ui->onionSkinningSelector->setCheckable(true);
    ui->ditherSelector->setCheckable(true);
    ui->paintBucketSelector->setCheckable(true);
    ui->eraserSelector->setCheckable(true);

    ui->deleteFrameButton->setDisabled(true);

    connect(ui->brushSelector,
            &QPushButton::clicked,
            this,
            &MainWindow::setBrushToggledState);

    connect(ui->brushSelector,
            &QPushButton::clicked,
            _model.get(),
            &Model::brushSelectedState);

    connect(ui->onionSkinningSelector,
            &QPushButton::clicked,
            this,
            &MainWindow::setOnionSkinningToggledState);

    connect(ui->onionSkinningSelector,
            &QPushButton::clicked,
            _model.get(),
            &Model::onionSkinningSelectedState);

    connect(ui->paintBucketSelector,
            &QPushButton::clicked,
            this,
            &MainWindow::setPaintBucketToggledState);

    connect(ui->paintBucketSelector,
            &QPushButton::clicked,
            _model.get(),
            &Model::paintBucketSelectedState);

    connect(ui->eraserSelector,
            &QPushButton::clicked,
            this,
            &MainWindow::setEraserToggledState);

    connect(ui->eraserSelector,
            &QPushButton::clicked,
            _model.get(),
            &Model::eraserSelectedState);

    connect(ui->ditherSelector,
            &QPushButton::clicked,
            this,
            &MainWindow::setDithererToggledState);

    connect(ui->ditherSelector,
            &QPushButton::clicked,
            _model.get(),
            &Model::dithererSelectedState);

    ui->BrushSizeSpinBox->setRange(1, 5);

    showColorOnButton(model->paintSettings.getPrimaryColor(), ui->primaryColor);
    connect(ui->primaryColor,
            &QPushButton::clicked,
            this,
            &MainWindow::primaryColorClicked);

    showColorOnButton(model->paintSettings.getSecondaryColor(), ui->secondaryColor);
    connect(ui->secondaryColor,
            &QPushButton::clicked,
            this,
            &MainWindow::secondaryColorClicked);

    connect(ui->BrushSizeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            _model.get(),
            &Model::brushSizeValueChanged);

    ui->actionSave->setShortcut(QKeySequence::Save);
    connect(ui->actionSave,
            &QAction::triggered,
            _model.get(),
            &Model::save);

    ui->actionSaveAs->setShortcut(QKeySequence::SaveAs);
    connect(ui->actionSaveAs,
            &QAction::triggered,
            _model.get(),
            &Model::saveAs);

    ui->actionLoad->setShortcut(QKeySequence::Open);
    connect(ui->actionLoad,
            &QAction::triggered,
            _model.get(),
            &Model::load);

    ui->action_Undo->setShortcut(QKeySequence::Undo);
    connect(ui->action_Undo,
            &QAction::triggered,
            _model.get(),
            &Model::undo);
    connect(_model.get(),
            &Model::updateUndoDisabled,
            ui->action_Undo,
            &QAction::setDisabled);
    ui->action_Undo->setDisabled(model->getUndoDisabled());

    ui->action_Redo->setShortcut(QKeySequence::Redo);
    connect(ui->action_Redo,
            &QAction::triggered,
            _model.get(),
            &Model::redo);
    connect(_model.get(),
            &Model::updateRedoDisabled,
            ui->action_Redo,
            &QAction::setDisabled);
    ui->action_Redo->setDisabled(model->getRedoDisabled());

    //Mouse Event Connections
    connect(this,
            &MainWindow::mouseClicked,
            _model.get(),
            &Model::mouseClicked);

    connect(this,
            &MainWindow::mouseMoved,
            _model.get(),
            &Model::mouseMoved);

    //Window Event Connections
    connect(this,
            &MainWindow::windowResized,
            this,
            &MainWindow::drawCurrentFrame);

    connect(ui->confirmDimensionsButton,
            &QDialogButtonBox::accepted,
            this,
            &MainWindow::changeFrameDimensions);
    //connect QVector of frames to QListView
    ui->frameList->setModel(&model->sprite);
    ui->frameList->setItemDelegate(new FrameItemDelegate);
    //set framslist can drag their elements
    ui->frameList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->frameList->setDragEnabled(true);
    ui->frameList->setAcceptDrops(true);
    ui->frameList->setDropIndicatorShown(true);
    ui->frameList->setDragDropMode(QAbstractItemView::DragDropMode::InternalMove);

    connect(&model->sprite,
            &Animation::drawFrame,
            this,
            &MainWindow::selectFrame);

    model->sprite.changeFrame(0);

    connect(ui->addFrameButton,
            &QPushButton::clicked,
            _model.get(),
            &Model::addFrameToList);

    ui->deleteFrameButton->setDisabled(true);

    connect(ui->deleteFrameButton,
            &QPushButton::clicked,
            _model.get(),
            &Model::deleteFrameFromList);

    connect(&model->sprite, &Animation::disableDeleteButton,
            ui->deleteFrameButton,
            &QPushButton::setDisabled);

    connect(ui->frameList, &QListView::clicked,
            _model.get(),
            //change big frame based on preview frame clicked
            &Model::frameClicked);

    //when button of copy frame is clicked, invoke copyFrame method
    connect(ui->copyFrame, &QPushButton::clicked,
            _model.get(),
            &Model::copyFrame);

    connect(_model.get(),
            &Model::showWarning,
            this,
            &MainWindow::showWarning);

    ui->animationPreviewButton->setDisabled(true);

    connect(&model->sprite,
            &Animation::setStateofAnimationPreview,
            ui->animationPreviewButton,
            &QPushButton::setDisabled);

    connect(ui->animationPreviewButton,
            &QPushButton::clicked,
            _model.get(),
            &Model::setStartingAnimationFrame);
}

///
/// \brief MainWindow::~MainWindow destructor.
///
MainWindow::~MainWindow()
{
    delete ui;
}

///
/// \brief MainWindow::drawCurrentFrame Draws the current frame with all settings
///
void MainWindow::drawCurrentFrame()
{
    QImage curFrame = model->sprite.getCurFrame()->canvas;

    //If onion skinning selected, draw partially transparent previous frame on screen.
    if (model->getOnionSkinningSelected())
    {
        QImage prevFrame = model->sprite.getPrevFrame()->canvas;
        QPainter painter;
        painter.begin(&curFrame);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.setOpacity(0.4);
        painter.drawImage(curFrame.rect(), prevFrame);
        painter.end();
    }

    //Create background for drawable area
    QImage background(curFrame.size(), QImage::Format_ARGB32);
    background.fill(QColor(255, 255, 255));
    QPainter painter;
    painter.begin(&curFrame);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    painter.drawImage(curFrame.rect(), background);
    painter.end();

    //Scale pixel image to be correct size on screen
    QSize labelSize = ui->frameLabel->size();
    double frameAspect = (double)curFrame.height() / (double)curFrame.width();
    double labelAspect = (double)labelSize.height() / (double)labelSize.width();
    if(frameAspect >= labelAspect)
    {
        QSize pixmapSize(curFrame.width() * labelSize.height() / curFrame.height(),
                         labelSize.height());
        QPixmap pixmap = QPixmap::fromImage(curFrame.scaled(pixmapSize));
        ui->frameLabel->setPixmap(pixmap);
    }
    else
    {
        QSize pixmapSize(labelSize.width(),
                         curFrame.height() * labelSize.width() / curFrame.width());
        QPixmap pixmap = QPixmap::fromImage(curFrame.scaled(pixmapSize));
        ui->frameLabel->setPixmap(pixmap);
    }
}

///
/// \brief MainWindow::selectFrame slot run when a different frame is selected
/// \param currFrame the frame that is now selected
///
void MainWindow::selectFrame(std::shared_ptr<Frame> currFrame)
{
    //draws frame clicked at the bottom
    drawCurrentFrame();
    connect(&(*currFrame), &Frame::canvasChanged, this, &MainWindow::drawCurrentFrame);
}

///
/// \brief MainWindow::mousePressEvent Event that is fired when the mouse is clicked on the frame
/// \param event - the mouse position
///
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(ui->frameLabel->frameGeometry().contains(event->pos()))
    {
        int offCenterX = (ui->frameLabel->width() - ui->frameLabel->pixmap().width()) / 2;
        int offCenterY = (ui->frameLabel->height() - ui->frameLabel->pixmap().height()) / 2;
        int frameX = event->pos().x() - ui->frameLabel->x() - offCenterX;
        int frameY = event->pos().y() - ui->frameLabel->y() - offCenterY;
        int pixelX = frameX * model->sprite.getCurFrame()->getFrameWidth()/ui->frameLabel->pixmap().width();
        int pixelY = frameY * model->sprite.getCurFrame()->getFrameHeight()/ui->frameLabel->pixmap().height();
        prevX = pixelX;
        prevY = pixelY;
        emit mouseClicked(pixelX, pixelY);
    }
}

///
/// \brief MainWindow::mouseMoveEvent The event that is fired when the mouse is moved
/// \param event - the mouse position
///
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(ui->frameLabel->frameGeometry().contains(event->pos()))
    {
        int offCenterX = (ui->frameLabel->width() - ui->frameLabel->pixmap().width()) / 2;
        int offCenterY = (ui->frameLabel->height() - ui->frameLabel->pixmap().height()) / 2;
        int frameX = event->pos().x() - ui->frameLabel->x() - offCenterX;
        int frameY = event->pos().y() - ui->frameLabel->y() - offCenterY;
        int pixelX = frameX * model->sprite.getCurFrame()->getFrameWidth()/ui->frameLabel->pixmap().width();
        int pixelY = frameY * model->sprite.getCurFrame()->getFrameHeight()/ui->frameLabel->pixmap().height();
        emit mouseMoved(pixelX, pixelY, prevX, prevY);
        prevX = pixelX;
        prevY = pixelY;
    }
}

///
/// \brief MainWindow::changeFrameDimensions changes the frames dimensions
///
void MainWindow::changeFrameDimensions()
{
    model->sprite.changeFrameDimensions(ui->frameDimensionWidth->value(), ui->frameDimensionHeight->value());
    drawCurrentFrame();
}

// resize frame display when window is resized
void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    emit windowResized();
}

// turn off all the tool buttons visually
void MainWindow::clearToolToggles()
{
    ui->brushSelector->setChecked(false);
    ui->paintBucketSelector->setChecked(false);
    ui->eraserSelector->setChecked(false);
}

// slot to select the brush tool visually
void MainWindow::setBrushToggledState(bool clicked)
{
    clearToolToggles();
    ui->brushSelector->setChecked(clicked);
}

// slot to toggle onion skinning visually
void MainWindow::setOnionSkinningToggledState(bool clicked)
{
    ui->onionSkinningSelector->setChecked(clicked);
}

// slot to select the paint bucket tool visually
void MainWindow::setPaintBucketToggledState(bool clicked)
{
    clearToolToggles();
    ui->paintBucketSelector->setChecked(clicked);
}

// slot to select the eraser tool visually
void MainWindow::setEraserToggledState(bool clicked)
{
    clearToolToggles();
    ui->eraserSelector->setChecked(clicked);
}

// slot to toggle the dithering feature visually
void MainWindow::setDithererToggledState(bool clicked)
{
    ui->ditherSelector->setChecked(clicked);
}

// open color picker and change the primary color
void MainWindow::primaryColorClicked()
{
    QColor primary = QColorDialog::getColor();
    model->paintSettings.setPrimaryColor(primary);
    showColorOnButton(primary, ui->primaryColor);
}

// open color picker and change secondary color
void MainWindow::secondaryColorClicked()
{
    QColor secondary = QColorDialog::getColor();
    model->paintSettings.setSecondaryColor(secondary);
    showColorOnButton(secondary, ui->secondaryColor);
}

// helper to change the color of a QPushButton
void MainWindow::showColorOnButton(const QColor &color, QPushButton *button)
{
    // https://stackoverflow.com/a/15081186
    QString s("background: #"
          + QString(color.red() < 16? "0" : "") + QString::number(color.red(),16)
          + QString(color.green() < 16? "0" : "") + QString::number(color.green(),16)
          + QString(color.blue() < 16? "0" : "") + QString::number(color.blue(),16) + ";");
    button->setStyleSheet(s);
    button->update();
}

// helper to show a warning message box
void MainWindow::showWarning(const QString& title, const QString& text)
{
    QMessageBox::warning(this, title, text);
}

// show animation preview window
void MainWindow::on_animationPreviewButton_clicked()
{
    animationPreview.setModal(true);
    animationPreview.exec();
}
