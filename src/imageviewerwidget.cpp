#include "imageviewerwidget.h"

#include <QShortcut>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QtConcurrent>
#include <QMessageBox>
#include <iterator>
#include <algorithm>

ImageViewerWidget::ImageViewerWidget(QWidget* parent)
    : QWidget(parent)
    , currentShowImageData_(nullptr)
    , pixmap_widget(new QWidget(this))
    , pixmap_layout(new QHBoxLayout())
{
    pixmap_ = new AutoScalePixmapLabel();
    pixmap_->setAlignment(Qt::AlignCenter);

    QVBoxLayout* main_part_layout = new QVBoxLayout();

    main_part_layout->addWidget(pixmap_widget);
    pixmap_layout->addWidget(pixmap_, 1);


    QHBoxLayout* button_layout = new QHBoxLayout();
    QHBoxLayout* button_companator = new QHBoxLayout();

    QPushButton* prev_image_button = new QPushButton("Prev");
    QPushButton* next_image_button = new QPushButton("Next");
    button_companator->addWidget(prev_image_button);
    button_companator->addWidget(next_image_button);
    button_layout->addLayout(button_companator);

    QSize size_button {50, 30};
    prev_image_button->resize(size_button);
    prev_image_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    next_image_button->resize(size_button);
    next_image_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

    main_part_layout->addLayout(button_layout);
    setLayout(main_part_layout);

    connect(prev_image_button, &QPushButton::pressed, this, &ImageViewerWidget::onPrevImage);
    QShortcut *shortcut_prev_A = new QShortcut(QKeySequence(Qt::Key_A), this);
    QShortcut *shortcut_prev_Left = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(shortcut_prev_A, &QShortcut::activated, this, &ImageViewerWidget::onPrevImage);
    connect(shortcut_prev_Left, &QShortcut::activated, this, &ImageViewerWidget::onPrevImage);

    connect(next_image_button, &QPushButton::pressed, this, &ImageViewerWidget::onNextImage);
    QShortcut *shortcut_next_D = new QShortcut(QKeySequence(Qt::Key_D), this);
    QShortcut *shortcut_prev_Right = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(shortcut_next_D, &QShortcut::activated, this, &ImageViewerWidget::onNextImage);
    connect(shortcut_prev_Right, &QShortcut::activated, this, &ImageViewerWidget::onNextImage);
}


ImageViewerWidget::~ImageViewerWidget()
{
    delete[] currentShowImageData_;
}



void ImageViewerWidget::getPathToImagesFile(const QFileInfoList& files)
{
    pathesToImages_.clear();
    for(const QFileInfo& file : files) {
        pathesToImages_.emplaceBack(file.absoluteFilePath());
    }

    if(pathesToImages_.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "This catalog does not have pictures. You can choose another catalog");
        return;
    }
    curImagePathIt_ = pathesToImages_.begin();
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
}


void ImageViewerWidget::changeLightnessImage(int delta)
{
    delete[] currentShowImageData_;
    currentShowImageData_ = parallelChangeLightness(currentShowImage_, delta);
    QImage image(currentShowImageData_, currentShowImage_.width(), currentShowImage_.height(), currentShowImage_.format());
    showNewImage(image);
}



void ImageViewerWidget::showNewImage(const QImage& image)
{
    QPixmap new_pixmap = QPixmap::fromImage(image);
    pixmap_->setPixmap(new_pixmap);
}


void ImageViewerWidget::onNextImage()
{
    ++curImagePathIt_;
    curImagePathIt_ = curImagePathIt_ == pathesToImages_.end() ? pathesToImages_.begin() : curImagePathIt_;
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
    emit showImage();
}


void ImageViewerWidget::onPrevImage()
{
    curImagePathIt_ = curImagePathIt_ == pathesToImages_.begin() ?
                          --pathesToImages_.end() : --curImagePathIt_;
    currentShowImage_.load(*curImagePathIt_);
    showNewImage(currentShowImage_);
    emit showImage();
}


uchar* parallelChangeLightness(const QImage& image, int delta)
{
    int image_height = image.height();
    qsizetype bytes_per_line = image.bytesPerLine();
    uchar* data = new uchar [bytes_per_line * image_height];
    int count_threads = std::max(QThreadPool::globalInstance()->maxThreadCount() / 2, 1);
    int part_size = image_height / (count_threads);
    for(int cur_thread = 0; cur_thread < count_threads; ++cur_thread)
    {
        {
            QtConcurrent::task([cur_thread, part_size](const QImage& image, uchar* data, int delta)
                               {
                                   qsizetype bytes_per_line = image.bytesPerLine();
                                   for(int index = cur_thread * part_size, max_index = (cur_thread + 1) * part_size; index < max_index; ++index){
                                       const uchar* data_line = image.constScanLine(index);
                                       for (qsizetype x = 0; x < bytes_per_line; ++x)
                                       {
                                           int new_pixel_int = (int)data_line[x] + delta;
                                           data[index * bytes_per_line + x] = new_pixel_int > 255 ?  255 : new_pixel_int < 0 ? 0 : new_pixel_int;
                                       }
                                   }
                               }).withArguments(image, data, delta)
                .onThreadPool(*QThreadPool::globalInstance())
                .spawn(QtConcurrent::FutureResult::Ignore);
        }
    }
    for(int index = image_height % count_threads; index > 0; --index)
    {
        const uchar* data_line = image.constScanLine(image_height - index);
        for (qsizetype x = 0; x < bytes_per_line; ++x)
        {
            int new_pixel_int = (int)data_line[x] + delta;
            data[(image_height - index) * bytes_per_line + x] = new_pixel_int > 255 ?  255 : new_pixel_int < 0 ? 0 : new_pixel_int;
        }
    }
    QThreadPool::globalInstance()->waitForDone();
    return data;
}


uchar* nonParallelChangeLightness(const QImage& image, int delta)
{
    int image_height = image.height();
    qsizetype bytes_per_line = image.bytesPerLine();
    uchar* data = new uchar [bytes_per_line * image_height];
    for (qsizetype y = 0; y < image_height; ++y)
    {
        const uchar* data_line = image.constScanLine(y);
        for (qsizetype x = 0; x < bytes_per_line; ++x)
        {
            int new_pixel_int = (int)data_line[x] + delta;
            data[y * bytes_per_line + x] = new_pixel_int > 255 ?  255 : new_pixel_int < 0 ? 0 : new_pixel_int;
        }

    }
    return data;
}
