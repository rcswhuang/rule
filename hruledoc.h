#ifndef HRULEDOC_H
#define HRULEDOC_H

#include <QObject>
/*
 * 所有文件操作都存放如此
*/
#include "rulefile.h"
class HRuleDoc
{
public:
    HRuleDoc();
    ~HRuleDoc();
public:
    HRuleFile* getRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType,quint16 wRuleID, quint8 btRuleType, QString &strFormula);
    bool loadRuleFiles();
    void saveRuleFiles();
public:
    //extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
    //extern quint8 m_btAppType;
    QString m_strRuleFilePath;
    HStationRuleList m_StationRuleList;

};

#endif // HRULEDOC_H
