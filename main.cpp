#include "hrulewindow.h"
#include <QApplication>
#include "hrulefile.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    HRuleFile* rf = new HRuleFile;
    HRuleWindow w(rf);
    w.show();

    return a.exec();
}
