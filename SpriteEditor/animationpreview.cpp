//Code style reviewed by Joey Cai on 4/5/2023
#include "animationpreview.h"
#include "ui_animationpreview.h"

///constructor sets up intital view and connections
AnimationPreview::AnimationPreview(std::shared_ptr<Model> _model, QWidget *parent)
    : QDialog(parent),
        ui(new Ui::AnimationPreview),
        modelForAnimation(_model)
{
    ui->setupUi(this);

    //sets the max fps
    ui->fpsSlider->setMaximum(24);

    //Changes displaed LCD value based on where slider was dragged
    connect(ui->fpsSlider,
            &QAbstractSlider::sliderMoved,
            ui->fpsDisplay,
            QOverload<int>::of(&QLCDNumber::display));

    //Start animation when slider is dragged
    connect(ui->fpsSlider,
            &QAbstractSlider::sliderMoved,
            _model.get(),
            &Model::playAnimation);

    //Draw a new frame when the frame is changed
    connect(&modelForAnimation->sprite,
            &Animation::drawAnimationFrame,
            this,
            &AnimationPreview::drawCurrentFrame);

    //Update max amount of frames the frame picker slider can go to based on how many frames have been made
    connect(_model.get(),
            &Model::numOfMadeFrames,
            this,
            &AnimationPreview::updateSliderMax);

    //change frame shown based on where slider was dragged
    connect(ui->currFrameSlider,
            &QAbstractSlider::sliderMoved,
            _model.get(),
            &Model::changeFrameShown);

    //Stop animation when preview window is closed
    connect(this,
            &QDialog::finished,
            _model.get(),
            &Model::stopTimer);

    //reset fps display and slider when preview window is closed to reflect the timer stopping
    connect(this,
            &QDialog::finished,
            this,
            &AnimationPreview::animationPreviewWindowClosed);
}

///Destructor for ui object
AnimationPreview::~AnimationPreview()
{
    delete ui;
}

///Displays whatever frame is current in preview
void AnimationPreview::drawCurrentFrame()
{
    QImage image = modelForAnimation->sprite.getCurFrame()->canvas;
    QPixmap pixmap = QPixmap::fromImage(image.scaled(ui->preivewAnimationLabel->size(), Qt::KeepAspectRatio));
    ui->preivewAnimationLabel->setPixmap(pixmap);
}

///update max frames slider can go to based on how many frames were made by user
void AnimationPreview::updateSliderMax(int max)
{
    ui->currFrameSlider->setMaximum(max);
}

//reset fps display and slider to zero
void AnimationPreview::animationPreviewWindowClosed()
{
    ui->fpsSlider->setValue(0);
    ui->fpsDisplay->display(0);
}

