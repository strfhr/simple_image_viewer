#include "brightnessslider.h"

extern const int FULL_BLACK = -255;
extern const int FULL_WHITE = 255;
extern const int MIN_STEP = 10;
extern const int ZERO = 0;

BrightnessSlider::BrightnessSlider(QWidget* parent = nullptr)
    :QSlider(Qt::Vertical, parent)
{
    this -> setMinimum(FULL_BLACK);
    this -> setMaximum(FULL_WHITE);
    this -> setSingleStep(MIN_STEP);
}


void BrightnessSlider::resetSlider()
{
    setValue(ZERO);
}
