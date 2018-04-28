#include "hrulewindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HRuleWindow w;
    w.show();

    return a.exec();
}
