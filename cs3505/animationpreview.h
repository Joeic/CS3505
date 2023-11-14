//Code style reviewed by Joey Cai on 4/5/2023
#ifndef ANIMATIONPREVIEW_H
#define ANIMATIONPREVIEW_H

#include "model.h"
#include <QDialog>
#include <QLCDNumber>
#include <QSlider>

///This class represents the view of the animation preview window
/// Ayden Smith
/// 4/5/23
QT_BEGIN_NAMESPACE
namespace Ui
{
    class AnimationPreview;
}
QT_END_NAMESPACE

class AnimationPreview : public QDialog
{
    Q_OBJECT

public:
    explicit AnimationPreview(std::shared_ptr<Model> _model, QWidget *parent = nullptr);
    ~AnimationPreview();

private slots:
    void drawCurrentFrame();
    void updateSliderMax(int max);
    void animationPreviewWindowClosed();

private:
    Ui::AnimationPreview *ui;
    std::shared_ptr<Model> modelForAnimation;
};

#endif // ANIMATIONPREVIEW_H
