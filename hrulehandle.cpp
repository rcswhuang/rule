#include "hrulehandle.h"
#include <QApplication>
#include "mainwindow.h"
LPRULEDATACALLBACK m_lpRuleDataCallBack = NULL;
quint8 m_btAppType = -1;
QString strRuleFilePath = "";

HRuleHandle* HRuleHandle::m_pInstance = NULL;
HRuleHandle* HRuleHandle::Initstance()
{
    if(!m_pInstance)
    {
        m_pInstance = new HRuleHandle;
    }

    return m_pInstance;
}

void HRuleHandle::Exitstance()
{

}


HRuleHandle::HRuleHandle()
{

}

HRuleHandle::~HRuleHandle()
{

}

bool  HRuleHandle::initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack)
{
    m_btAppType = btType;
    strRuleFilePath = QString(szFilePath);
    m_lpRuleDataCallBack = lpDataCallBack;
    return true;
}

void  HRuleHandle::exitRuleFiles()
{

}

void  HRuleHandle::openRuleWindow()
{
    int argc = 1;
    QApplication* a = new QApplication(argc, NULL);
    MainWindow *w = new MainWindow;
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->setWindowModality(Qt::ApplicationModal);
    w->show();
    a->exec();
}



//////////////////////////////////////////////////////规则接口///////////////////////////////////////////////////////////

bool RULE_EXPORT initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack)
{
    return HRuleHandle::Initstance()->initRuleFiles(btType,szFilePath,lpDataCallBack);
}

void RULE_EXPORT exitRuleFiles()
{
    HRuleHandle::Initstance()->exitRuleFiles();
}

void RULE_EXPORT openRuleWindow()
{
    HRuleHandle::Initstance()->openRuleWindow();
}
