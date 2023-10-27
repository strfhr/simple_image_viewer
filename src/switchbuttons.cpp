#include "switchbuttons.h"

#include <QPushButton>
#include <QShortcut>
#include <QHBoxLayout>

SwitchButtons::SwitchButtons(QSize sizeButtons = {30, 50}, QWidget* parent = nullptr)
    : QWidget(parent)
    , prevButton_(new QPushButton("Prev", this))
    , nextButton_(new QPushButton("Next", this))
    , shortcutPrevA_(new QShortcut(QKeySequence(Qt::Key_A), this))
    , shortcutPrevLeft_(new QShortcut(QKeySequence(Qt::Key_Left), this))
    , shortcutNextD_(new QShortcut(QKeySequence(Qt::Key_D), this))
    , shortcutPrevRight_(new QShortcut(QKeySequence(Qt::Key_Right), this))
    , buttonsCompanator_(new QHBoxLayout(this))
{
    setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));

    prevButton_->resize(sizeButtons);
    nextButton_->resize(sizeButtons);

    buttonsCompanator_->setAlignment(Qt::AlignHCenter);
    buttonsCompanator_->addWidget(prevButton_);
    buttonsCompanator_->addWidget(nextButton_);

    connect(prevButton_, &QPushButton::pressed, this, &SwitchButtons::onPressedPrevButton);
    connect(shortcutPrevA_, &QShortcut::activated, this, &SwitchButtons::onPressedPrevButton);
    connect(shortcutPrevLeft_, &QShortcut::activated, this, &SwitchButtons::onPressedPrevButton);

    connect(nextButton_, &QPushButton::pressed, this, &SwitchButtons::onPressedNextButton);
    connect(shortcutNextD_, &QShortcut::activated, this, &SwitchButtons::onPressedNextButton);
    connect(shortcutPrevRight_, &QShortcut::activated, this, &SwitchButtons::onPressedNextButton);
}

void SwitchButtons::onPressedPrevButton()
{
    emit Prev();
}

void SwitchButtons::onPressedNextButton()
{
    emit Next();
}
