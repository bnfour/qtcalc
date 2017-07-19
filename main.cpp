#include "dialog.h"
#include <QApplication>

// basic Qt app init
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();

}
