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

    void  openRuleWindow();
};

#endif // HRULEHANDLE_H
