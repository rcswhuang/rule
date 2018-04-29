#include "hrulehandle.h"
#include "hrulewindow.h"
#include "rulefile.h"
LPRULEDATACALLBACK m_lpRuleDataCallBack = NULL;
quint8 m_btAppType = -1;
QString strRuleFilePath = "";
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
