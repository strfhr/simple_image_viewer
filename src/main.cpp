#include <QApplication>
#include <QFIle>
#include <QMessageBox>

#include "imageviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file(":/style.css");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(file.readAll());
    int result{};
    try {
        ImageViewer w;
        w.show();
        result = a.exec();
    } catch(const std::bad_alloc&) {
        QMessageBox::critical(nullptr, "ERROR", "Do not have memory for correct work.");
        result = 1;
    }
    return result;
}
