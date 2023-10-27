#ifndef AUTOSCALEPIXMAPLABEL_H
#define AUTOSCALEPIXMAPLABEL_H

#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

class AutoScalePixmapLabel : public QLabel
{
    Q_OBJECT

public:
    explicit AutoScalePixmapLabel(QWidget *parent = 0);
    virtual int heightForWidth( int width ) const;
    virtual QSize sizeHint() const;
    QPixmap scaledPixmap() const;

public slots:
    void setPixmap ( const QPixmap &);
    void resizeEvent(QResizeEvent *);

private:
    QPixmap pixmap_;
};

#endif // AUTOSCALEPIXMAPLABEL_H
