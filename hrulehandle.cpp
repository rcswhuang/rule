#include "hrulehandle.h"
#include "hrulewindow.h"
#include "hruledoc.h"
#include "rulefile.h"
#include "hruledoc.h"
LPRULEDATACALLBACK m_lpRuleDataCallBack = NULL;
quint8 m_btAppType = -1;
QString g_strRuleFilePath = "";
HStationRuleList g_StationRuleList;

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
    m_pRuleDoc = new HRuleDoc;
    m_pRuleDoc->loadRuleFiles();
}

HRuleHandle::~HRuleHandle()
{
    if(m_pRuleDoc)
    {
        delete m_pRuleDoc;
        m_pRuleDoc = NULL;
    }
}

bool  HRuleHandle::initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack)
{
    m_btAppType = btType;
    g_strRuleFilePath = QString(szFilePath);
    m_lpRuleDataCallBack = lpDataCallBack;
    return true;
}

void  HRuleHandle::exitRuleFiles()
{
//
}

void  HRuleHandle::openRuleWindow(quint16 wStationNo, //厂站ID
                                  quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                                  quint16 wPointNo,  //测点ID
                                  quint8  btYKType, //分，合，检修分，检修合
                                  quint16 wRuleID, //规则ID
                                  quint8 btRuleType, //规则类型
                                  QString &strFormula //公式字符串
                                  )
{
    m_pRuleDoc->loadRuleFiles();
    HRuleFile* pRuleFile = doc->getRuleFile(wStationNo,wPointType,wPointNo,btYKType,wRuleID,btRuleType,strFormula);
    if(!pRuleFile) return;
    HRuleWindow w(pRuleFile);
    w.exec();
}

void HRuleHandle::exportAllRule(quint16 wStationNo)
{
    m_pRuleDoc->exportAllRule(wStationNo);
}

bool HRuleHandle::isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    return m_pRuleDoc->isRuleFileExist(wStationNo,wPointType,wPointNo,btYKType);
}

bool HRuleHandle::delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    return m_pRuleDoc->delRuleFile(wStationNo,wPointType,wPointNo,btYKType);
}

void HRuleHandle::changeStationID(quint16 wStNo,quint16 wNewStNo)
{
    m_pRuleDoc->changeStationID(wStNo,wNewStNo);
}

void HRuleHandle::setRuleModify(bool modify)
{
    m_pRuleDoc->setRuleModify(modify);
}

void HRuleHandle::lookRuleReport(quint16 wStationNo,quint16 wPointNo)
{
    m_pRuleDoc->lookRuleReport(wStationNo,wPointNo);
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

void RULE_EXPORT openRuleWindow(quint16 wStationNo, //厂站ID
                                quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                                quint16 wPointNo,  //测点ID
                                quint8  btRelayType, //分，合，检修分，检修合
                                quint16 wRuleID, //规则ID
                                quint8 btType, //规则类型
                                QString &strFormula //公式字符串
                                )
{
    HRuleHandle::Initstance()->openRuleWindow(wStationNo,wPointType,wPointNo,btRelayType,wRuleID,btType,strFormula);
}


void RULE_EXPORT exportAllRule(quint16 wStationNo)
{
    HRuleHandle::Initstance()->exportAllRule(wStationNo);
}

bool RULE_EXPORT isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    return HRuleHandle::Initstance()->isRuleFileExist(wStationNo,wPointType,wPointNo,btYKType);
}

bool RULE_EXPORT delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    return HRuleHandle::Initstance()->delRuleFile(wStationNo,wPointType,wPointNo,btYKType);
}

void RULE_EXPORT changeStationID(quint16 wStNo,quint16 wNewStNo)
{
    HRuleHandle::Initstance()->changeStationID(wStNo,wNewStNo);
}

void RULE_EXPORT setRuleModify(bool modify)
{
    HRuleHandle::Initstance()->setRuleModify(modify);
}

void RULE_EXPORT lookRuleReport(quint16 wStationNo,quint16 wPointNo)
{
    HRuleHandle::Initstance()->lookRuleReport(wStationNo,wPointNo);
}
