#include "hrulehandle.h"
#include "hrulewindow.h"
#include "hruledoc.h"
#include "hrulefile.h"
#include "hruledoc.h"
#include "hlookrulereport.h"
#include "hexportrule.h"
LPRULEDATACALLBACK m_lpRuleDataCallBack = NULL;
quint8 m_btAppType = -1;

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
    if(m_pInstance)
    {
        delete m_pInstance;
        m_pInstance = NULL;
    }
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

bool  HRuleHandle::initRuleFiles(quint8 btType,LPRULEDATACALLBACK lpDataCallBack)
{
    if(!m_pRuleDoc)
        return false;
    //m_pRuleDoc->m_strRuleFilePath = QString(szFilePath);
    m_btAppType = btType;
    m_lpRuleDataCallBack = lpDataCallBack;
    return true;
}

void  HRuleHandle::exitRuleFiles()
{
    //保存规则
}

bool  HRuleHandle::openRuleWindow(quint16 wStationNo, //厂站ID
                                  quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                                  quint16 wPointNo,  //测点ID
                                  quint8  btYKType, //分，合，检修分，检修合
                                  quint16 wRuleID, //规则ID
                                  quint8 btRuleType, //规则类型
                                  QString &strFormula //公式字符串
                                  )
{
    if(!m_pRuleDoc)
        return false;
    HRuleFile* pRuleFile = m_pRuleDoc->getRuleFile(wStationNo,wPointType,wPointNo,btYKType,wRuleID,btRuleType,strFormula);
    if(!pRuleFile) return false;
    HRuleWindow w(pRuleFile);
    w.setRuleDoc(m_pRuleDoc);
    if(QDialog::Accepted == w.exec())
        return true;
    return false;
}

void HRuleHandle::exportAllRule()
{
    if(!m_pRuleDoc)
        return;
    HExportRule exRule(m_pRuleDoc);
    exRule.exec();

}

bool HRuleHandle::isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    if(!m_pRuleDoc)
        return false;
    return m_pRuleDoc->isRuleFileExist(wStationNo,wPointType,wPointNo,btYKType);
}

bool HRuleHandle::delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    if(!m_pRuleDoc)
        return false;
    return m_pRuleDoc->delRuleFile(wStationNo,wPointType,wPointNo,btYKType);
}

void HRuleHandle::changeStationID(quint16 wStNo,quint16 wNewStNo)
{
    if(!m_pRuleDoc)
        return;
    m_pRuleDoc->changeStationID(wStNo,wNewStNo);
}

void HRuleHandle::lookRuleReport(quint16 wStationNo,quint16 wPointNo)
{
    if(!m_pRuleDoc)
        return;
    HLookRuleReport look(m_pRuleDoc);
    look.setPoint(wStationNo,wPointNo);
    look.exec();
}


//////////////////////////////////////////////////////规则接口///////////////////////////////////////////////////////////

bool RULE_EXPORT initRuleFiles(quint8 btType,LPRULEDATACALLBACK lpDataCallBack)
{
    return HRuleHandle::Initstance()->initRuleFiles(btType,lpDataCallBack);
}

void RULE_EXPORT exitRuleFiles()
{
    HRuleHandle::Initstance()->Exitstance();
}

bool RULE_EXPORT openRuleWindow(quint16 wStationNo, //厂站ID
                                quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                                quint16 wPointNo,  //测点ID
                                quint8  btRelayType, //分，合，检修分，检修合
                                quint16 wRuleID, //规则ID
                                quint8 btType, //规则类型
                                QString &strFormula //公式字符串
                                )
{
    return HRuleHandle::Initstance()->openRuleWindow(wStationNo,wPointType,wPointNo,btRelayType,wRuleID,btType,strFormula);
}


void RULE_EXPORT exportAllRule()
{
    HRuleHandle::Initstance()->exportAllRule();
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

void RULE_EXPORT lookRuleReport(quint16 wStationNo,quint16 wPointNo)
{
    HRuleHandle::Initstance()->lookRuleReport(wStationNo,wPointNo);
}
