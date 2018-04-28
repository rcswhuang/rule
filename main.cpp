#include "mainwindow.h"
#include <QApplication>
LPRULEDATACALLBACK m_lpRuleDataCallBack = NULL;
quint8 m_btAppType = -1;
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
