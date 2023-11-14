#include "model.h"
#include <QDebug>
#include <QFileDialog>
#include <QJsonDocument>
#include <QMessageBox>

Model::Model(QWidget *parent) : QObject(parent), sprite(new Animation()), dialogParent(parent)
{
    paintSettings = Paint();
    currentTool = std::make_unique<Paintbrush>();

}


//void Model::brushSizeSelected(QString size)
//{
//    brushSize = size;
//}
bool Model::brushSelectedState()
{
    currentTool = std::make_unique<Paintbrush>();
    brushSelected = !brushSelected;
    paintBucketSelected = false;
    return brushSelected;
}

bool  Model::onionSkinningSelectedState()
{
    onionSkinningSelected = !onionSkinningSelected;
    return onionSkinningSelected;
}
bool  Model::dithererSelectedState()
{
    return paintSettings.switchDithering();
}
bool  Model::paintBucketSelectedState()
{
    currentTool = std::make_unique<PaintBucket>();
    paintBucketSelected = !paintBucketSelected;
    brushSelected = false;
    return paintBucketSelected;
}

void Model::brushSizeValueChanged(int value)
{
    paintSettings.setToolSize(value);
}

void Model::mouseClicked(int x, int y)
{
    currentTool->useToolAtSinglePoint(sprite.getCurFrame(), paintSettings, x, y);
}

void Model::mouseMoved(int x, int y)
{
    currentTool->useToolAtSinglePoint(sprite.getCurFrame(), paintSettings, x, y);
}

void Model::save()
{
    if(currentFile == tr(""))
        currentFile = pickSaveLocation("Save sprite...");
    if(currentFile == tr("")) // user cancelled
        return;

    writeToFile(currentFile);
}

void Model::saveAs()
{
    QString newFile = pickSaveLocation("Save sprite as...");
    if(newFile == tr("")) // user cancelled
        return;
    currentFile = newFile;

    writeToFile(currentFile);
}

bool Model::writeToFile(QString filename) {
    // We currently only support JSON, so use that.
    //   If we supported other formats, we would check the file extension here.

    QFile saveFile(filename);

    if(!saveFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(dialogParent, "Unable to write file", "Unable to open file for writing. Sprite is not saved.");
        return false;
    }

    QJsonObject ser = std::move(sprite.toJson());
    saveFile.write(QJsonDocument(ser).toJson(QJsonDocument::JsonFormat::Compact));

    return true;
}

void Model::load()
{
    QString openFilename = QFileDialog::getOpenFileName(dialogParent, "Open sprite...", QString(), tr("JSON files (*.ssp)"));
    if(openFilename == tr("")) // user cancelled
        return;

    currentFile = openFilename;
    QFile openFile(openFilename);

    if(!openFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(dialogParent, "Unable to open file", "Unable to open file for reading. Cannot open sprite.");
        return;
    }

    QByteArray loadData = openFile.readAll();
    QJsonDocument loadDoc = QJsonDocument::fromJson(loadData);
    QJsonObject s = loadDoc.object();

    sprite.loadJson(s);
}

QString Model::pickSaveLocation(const QString& caption)
{
    return QFileDialog::getSaveFileName(dialogParent, caption, QString(), tr("JSON files (*.ssp)"));
}

void Animation:: onAddFrame()
{

}
