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
    bool delRuleProFile(quint16 wStationNo);
    void exportAllRule(quint16 wStationNo);
    bool isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType);
    bool delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType);
    void changeStationID(quint16 wStID,quint16 wNewStID);
    void setRuleModify(bool modify);
    void lookRuleReport(quint16 wStation,quint16 wPointNo);
public:
    //extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
    //extern quint8 m_btAppType;
    bool m_bModify;
    QString m_strRuleFilePath;
    HStationRuleList* m_pStationRuleList;

};

#endif // HRULEDOC_H
