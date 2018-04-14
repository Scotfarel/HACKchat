#include "hackserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    hackserver w;
    w.show();

    return a.exec();
}
