#ifndef SWITCHBUTTONS_H
#define SWITCHBUTTONS_H

#include <QWidget>

class QPushButton;
class QShortcut;
class QHBoxLayout;

class SwitchButtons : public QWidget
{
    Q_OBJECT

public:
    SwitchButtons(QSize sizeButtons, QWidget* parent);

signals:
    void Prev();
    void Next();

private slots:
    void onPressedPrevButton();
    void onPressedNextButton();

private:
    QPushButton* prevButton_;
    QPushButton* nextButton_;

    QShortcut* shortcutPrevA_;
    QShortcut* shortcutPrevLeft_;
    QShortcut* shortcutNextD_;
    QShortcut* shortcutPrevRight_;

    QHBoxLayout* buttonsCompanator_;
};

#endif // SWITCHBUTTONS_H
