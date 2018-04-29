#ifndef HRULEHANDLE_H
#define HRULEHANDLE_H

#include <QObject>
#include "ruleeditapi.h"
class HRuleHandle
{
public:
    static HRuleHandle* Initstance();
    static void Exitstance();
    static HRuleHandle* m_pInstance;
    HRuleHandle();
    ~HRuleHandle();

public:

    bool  initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack);

    void  exitRuleFiles();

    void  openRuleWindow(quint16 wStationNo, //厂站ID
                         quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                         quint16 wPointNo,  //测点ID
                         quint8  btYKType, //控制类型:分，合，检修分，检修合
                         quint16 wRuleID, //规则ID
                         quint8 btRuleType, //规则类型:普通规则、二级规则
                         QString &strFormula //公式字符串
                         );
};

#endif // HRULEHANDLE_H
