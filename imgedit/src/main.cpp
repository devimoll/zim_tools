#include "imgedit.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    imgedit w;
    w.show();
    return a.exec();
}
