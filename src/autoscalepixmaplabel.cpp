#include "autoscalepixmaplabel.h"

AutoScalePixmapLabel::AutoScalePixmapLabel(QWidget* parent)
    : QLabel(parent)
{
    this->setMinimumSize(1,1);
    setScaledContents(false);
}


void AutoScalePixmapLabel::setPixmap(const QPixmap& newPixmap)
{
    pixmap_ = newPixmap;
    QLabel::setPixmap(scaledPixmap());
}


int AutoScalePixmapLabel::heightForWidth(int width) const
{
    return pixmap_.isNull() ? this->height() : ((qreal)pixmap_.height()*width)/pixmap_.width();
}


QSize AutoScalePixmapLabel::sizeHint() const
{
    int w = this->width();
    return QSize(w, heightForWidth(w));
}


QPixmap AutoScalePixmapLabel::scaledPixmap() const
{
    return pixmap_.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}


void AutoScalePixmapLabel::resizeEvent(QResizeEvent * e)
{
    if(!pixmap_.isNull())
        QLabel::setPixmap(scaledPixmap());
}
