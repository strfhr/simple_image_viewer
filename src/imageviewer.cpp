#include <QtConcurrent>
#include <QFileDialog>
#include <QMessageBox>
#include <QShortcut>
#include <QPushButton>
#include <QHBoxLayout>
#include <iterator>
#include <algorithm>
#include <memory>

#include "imageviewer.h"
#include "./ui_imageviewer.h"
#include "brightnessslider.h"
#include "switchbuttons.h"


const QStringList SUPPORTED_FORMATS {"*.jpg", "*.png", "*.bmp"};
constexpr QSize SIZE_OPEN_IMAGE_BUTTON {100, 50};
constexpr QSize SIZE_SWITCH_BUTTONS {50, 30};

ImageViewer::ImageViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ImageViewer)
    , openImageButton_(new QPushButton("Open Catalog", this))
    , switchButtons_(new SwitchButtons(SIZE_SWITCH_BUTTONS, this))
    , brightnessSlider_(new BrightnessSlider(this))
    , imagePixmap_(new AutoScalePixmapLabel(this))
    , imageWidget_(new QWidget(this))
    , currentShowImageData_(nullptr)
    , mainLayout_(new QHBoxLayout())
    , selectImageLayout_(new QVBoxLayout())
    , imageLayout_(new QHBoxLayout())
{
    ui->setupUi(this);

    mainLayout_->addWidget(openImageButton_, Qt::AlignCenter);
    openImageButton_->setMaximumSize(SIZE_OPEN_IMAGE_BUTTON);
    centralWidget()->setLayout(mainLayout_.get());

    imagePixmap_->setAlignment(Qt::AlignHCenter);
    imageWidget_->setLayout(imageLayout_.get());
    imageLayout_->addWidget(imagePixmap_, Qt::AlignCenter);
    selectImageLayout_->addWidget(imageWidget_, 1, Qt::AlignVCenter);
    imageWidget_->hide();

    selectImageLayout_->addWidget(switchButtons_);
    switchButtons_->hide();

    selectImageLayout_->setAlignment(Qt::AlignHCenter);

    mainLayout_->addLayout(selectImageLayout_.get());
    mainLayout_->addWidget(brightnessSlider_);
    brightnessSlider_->hide();

    connect(switchButtons_, &SwitchButtons::Prev, this, &ImageViewer::onPrevImage);
    connect(switchButtons_,  &SwitchButtons::Next, this, &ImageViewer::onNextImage);
    connect(brightnessSlider_, &QAbstractSlider::valueChanged, this, &ImageViewer::changeBrightnessImage);
    connect(this, &ImageViewer::showImage, brightnessSlider_, &BrightnessSlider::resetSlider);
    connect(ui->actionQuit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->actionOpen_Catalog, &QAction::triggered, this, &ImageViewer::onOpenFileDialog);
    connect(openImageButton_, &QPushButton::pressed, this, &ImageViewer::onOpenFileDialog);
    connect(this, &ImageViewer::showImage, this, &ImageViewer::onFirstImageOpen);

    currentDir_ = QDir::homePath();
}


ImageViewer::~ImageViewer()
{
    delete[] currentShowImageData_;
    delete ui;
}


void ImageViewer::onOpenFileDialog()
{
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Choose Directory"), currentDir_.absolutePath(),
                                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
                                                        | QFileDialog::DontUseNativeDialog);
    if(dirPath == "") return;
    currentDir_ = QDir(dirPath);
    getPathesToImagesFile(currentDir_.entryInfoList(SUPPORTED_FORMATS, QDir::Files | QDir::Readable));
    if(pathesToImages_.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "This catalog does not have pictures. You can choose another catalog");
        onOpenFileDialog();
        return;
    }
    curImagePathIt_ = pathesToImages_.begin();
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
    emit showImage();
}


void ImageViewer::getPathesToImagesFile(const QFileInfoList& files)
{
    pathesToImages_.clear();
    for(const QFileInfo& file : files) {
        pathesToImages_.emplaceBack(file.absoluteFilePath());
    }
}


void ImageViewer::showNewImage(const QImage& image)
{
    imagePixmap_->setPixmap(QPixmap::fromImage(image));
    imageWidget_->setMaximumSize(image.size());
}


void ImageViewer::onNextImage()
{
    ++curImagePathIt_;
    curImagePathIt_ = curImagePathIt_ == pathesToImages_.end() ? pathesToImages_.begin() : curImagePathIt_;
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
    emit showImage();
}


void ImageViewer::onPrevImage()
{
    curImagePathIt_ = curImagePathIt_ == pathesToImages_.begin() ?
                          --pathesToImages_.end() : --curImagePathIt_;
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
    emit showImage();
}


uchar* parallelChangeBrightness(const QImage& image, int delta);
uchar* nonParallelChangeBrightness(const QImage& image, int delta);

void ImageViewer::changeBrightnessImage(int delta)
{
    delete[] currentShowImageData_;
    try {
        currentShowImageData_ = parallelChangeBrightness(currentShowImage_, delta);
    } catch(const std::bad_alloc&) {
        QMessageBox::warning(this, "Warning", "Do not have add memory for this action.");
            return;
    }

    QImage image(currentShowImageData_, currentShowImage_.width(), currentShowImage_.height(), currentShowImage_.format());
    showNewImage(image);
}


void ImageViewer::onFirstImageOpen()
{
    openImageButton_->hide();

    imageWidget_->show();
    switchButtons_->show();
    brightnessSlider_->show();

    disconnect(this, &ImageViewer::showImage, this, &ImageViewer::onFirstImageOpen);
}


uchar* parallelChangeBrightness(const QImage& image, int delta)
{
    int imageHeight = image.height();
    qsizetype bytesPerLine = image.bytesPerLine();
    int countThreads = std::max(QThreadPool::globalInstance()->maxThreadCount() / 2, 1);
    if(countThreads == 1)
    {
        return nonParallelChangeBrightness(image, delta);
    }
    qsizetype partSize = imageHeight / countThreads;
    uchar* data = new uchar [bytesPerLine * imageHeight];
    for(qsizetype curThread = 0; curThread < countThreads; ++curThread)
    {
        {
            QtConcurrent::task([curThread, partSize](const QImage& image, uchar* data, int delta)
                               {
                                   qsizetype bytesPerLine = image.bytesPerLine();
                                   qsizetype index = curThread * partSize;
                                   qsizetype maxIndex = (curThread + 1) * partSize;
                                   for(;index < maxIndex; ++index){
                                       const uchar* dataLine = image.constScanLine(index);
                                       for (qsizetype x = 0; x < bytesPerLine; ++x)
                                       {
                                           int newPixelInt = (int)dataLine[x] + delta;
                                           qsizetype arrIndex = index * bytesPerLine + x;
                                           data[arrIndex] = newPixelInt > 255 ?  255 : newPixelInt < 0 ? 0 : newPixelInt;
                                       }
                                   }
                               }).withArguments(image, data, delta)
                .onThreadPool(*QThreadPool::globalInstance())
                .spawn(QtConcurrent::FutureResult::Ignore);
        }
    }
    for(int index = imageHeight % countThreads; index > 0; --index)
    {
        const uchar* dataLine = image.constScanLine(imageHeight - index);
        for (qsizetype x = 0; x < bytesPerLine; ++x)
        {
            int newPixelInt = (int)dataLine[x] + delta;
            qsizetype arrIndex = (imageHeight - index) * bytesPerLine + x;
            data[arrIndex] = newPixelInt > 255 ?  255 : newPixelInt < 0 ? 0 : newPixelInt;
        }
    }
    QThreadPool::globalInstance()->waitForDone();
    return data;
}


uchar* nonParallelChangeBrightness(const QImage& image, int delta)
{
    int imageHeight = image.height();
    qsizetype bytesPerLine = image.bytesPerLine();
    uchar* data = new uchar [bytesPerLine * imageHeight];
    for (qsizetype y = 0; y < imageHeight; ++y)
    {
        const uchar* dataLine = image.constScanLine(y);
        for (qsizetype x = 0; x < bytesPerLine; ++x)
        {
            int newPixelInt = (int)dataLine[x] + delta;
            qsizetype arrIndex = y * bytesPerLine + x;
            data[arrIndex] = newPixelInt > 255 ?  255 : newPixelInt < 0 ? 0 : newPixelInt;
        }

    }
    return data;
}
