// Code style reviewed by Kyle Holland on 4/5/2023
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "animationpreview.h"
#include "model.h"
#include <QMainWindow>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>

///This class represents the view of the pixel editor
/// Ayden Smith, Cameron Wortmann, Joey Cai, Kyle Holland
/// 4/5/23
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    MainWindow(std::shared_ptr<Model> _model , QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void setBrushToggledState(bool clicked);
    void setOnionSkinningToggledState(bool clicked);
    void setDithererToggledState(bool clicked);
    void setPaintBucketToggledState(bool clicked);
    void setEraserToggledState(bool clicked);
    void drawCurrentFrame();
    void selectFrame(std::shared_ptr<Frame> currFrame);

    void primaryColorClicked();
    void secondaryColorClicked();
    void changeFrameDimensions();

    void showWarning(const QString& title, const QString& text);

signals:
    void mouseClicked(int pixelX, int pixelY);
    void mouseMoved(int pixelX, int pixelY, int prevX, int prevY);
    void windowResized();

private slots:
    void on_animationPreviewButton_clicked();

private:
    int prevX;
    int prevY;
    Ui::MainWindow *ui;
    std::shared_ptr<Model> model;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    AnimationPreview animationPreview;
    void showColorOnButton(const QColor &color, QPushButton *button);

    void clearToolToggles();
};


#endif // MAINWINDOW_H
