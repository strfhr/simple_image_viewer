#ifndef IMAGEVIEWERWIDGET_H
#define IMAGEVIEWERWIDGET_H

#include <QWidget>
#include "autoscalepixmaplabel.h"
#include "imageviewerwidget.h"
#include <QDir>

class QHBoxLayout;
class QPushButton;

class ImageViewerWidget : public QWidget
{
    Q_OBJECT
public:
    ImageViewerWidget(QWidget* parent);
    virtual ~ImageViewerWidget();

    void showNewImage(const QImage& image);
    void changeLightnessImage(int delta);
    void getPathToImagesFile(const QFileInfoList& files);
private slots:
    void onNextImage();
    void onPrevImage();

signals:
    void showImage();

private:
    QPushButton* prevImageButton_;
    QPushButton* nextImageButton_;

    QImage currentShowImage_;
    uchar* currentShowImageData_;
    AutoScalePixmapLabel* pixmap_;
    QHBoxLayout* pixmap_layout;
    QWidget* pixmap_widget;

    QList<QString> pathesToImages_;
    QList<QString>::iterator curImagePathIt_;

};

uchar* parallelChangeLightness(const QImage& image, int delta);
uchar* nonParallelChangeLightness(const QImage& image, int delta);
#endif // IMAGEVIEWERWIDGET_H
