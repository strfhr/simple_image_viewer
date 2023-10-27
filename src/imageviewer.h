#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QMainWindow>
#include <QDir>

#include "autoscalepixmaplabel.h"

class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QFileDialog;
class BrightnessSlider;
class SwitchButtons;
class unique_ptr;

QT_BEGIN_NAMESPACE
namespace Ui { class ImageViewer; }
QT_END_NAMESPACE

extern const QStringList SUPPORTED_FORMATS;

class ImageViewer : public QMainWindow
{
    Q_OBJECT

public:
    ImageViewer(QWidget *parent = nullptr);
    ~ImageViewer();

private slots:
    void onOpenFileDialog();
    void onFirstImageOpen();
    void onNextImage();
    void onPrevImage();
    void changeBrightnessImage(int delta);

signals:
    void showImage();

private:
    Ui::ImageViewer *ui;

    QDir currentDir_;
    QList<QString> pathesToImages_;
    QList<QString>::iterator curImagePathIt_;
    QImage currentShowImage_;

    QPushButton* openImageButton_;
    SwitchButtons* switchButtons_;
    AutoScalePixmapLabel* imagePixmap_;
    QWidget* imageWidget_;
    BrightnessSlider* brightnessSlider_;
    uchar* currentShowImageData_;

    std::unique_ptr<QHBoxLayout> mainLayout_;
    std::unique_ptr<QVBoxLayout> selectImageLayout_;
    std::unique_ptr<QHBoxLayout> imageLayout_;

    void showNewImage(const QImage& image);
    void getPathesToImagesFile(const QFileInfoList& files);
};
#endif // IMAGEVIEWER_H
