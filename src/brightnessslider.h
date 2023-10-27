#ifndef BRIGHTNESSSLIDER_H
#define BRIGHTNESSSLIDER_H

#include <QWidget>
#include <QSlider>

extern const int FULL_BLACK;
extern const int FULL_WHITE;
extern const int MIN_STEP;
extern const int ZERO;

class BrightnessSlider : public QSlider
{
    Q_OBJECT

public:
    BrightnessSlider(QWidget* parent);

public slots:
    void resetSlider();
};

#endif // BRIGHTNESSSLIDER_H
