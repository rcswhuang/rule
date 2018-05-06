#ifndef HRULEHANDLE_H
#define HRULEHANDLE_H

#include <QObject>
#include "hruleeditapi.h"
class HRuleDoc;
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

    //导出规则
    void exportAllRule();

    //规则是否存在
    bool isRuleFileExist(quint16 wStationNo, //厂站ID
                         quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                         quint16 wPointNo,  //测点ID
                         quint8  btYKType //控制类型:分，合，检修分，检修合
                         );
    //导入规则（待定）
    //拷贝规则（待定）
    //删除规则

    bool delRuleFile(quint16 wStationNo, //厂站ID
                        quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                        quint16 wPointNo,  //测点ID
                        quint8  btYKType //控制类型:分，合，检修分，检修合
                        );
    //保存厂站规则（待定）

    //改变厂站ID
    void changeStationID(quint16 wStID,quint16 wNewStID);

    //改变装置ID（待定，如果是后台规则对象就是装置，但对五防，规则对象就是测点）

    //规则查看
    void lookRuleReport(quint16 wStation = (quint16)-1,quint16 wPointNo = (quint16)-1);
private:
    HRuleDoc* m_pRuleDoc;
};

#endif // HRULEHANDLE_H
