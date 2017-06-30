#ifndef HRULEHANDLE_H
#define HRULEHANDLE_H
#include <QObject>
#include "ruleeditapi.h"
#include "ruleexport.h"
//主要实现对外的接口部分
extern HRuleHandle* HRuleHandle::pInstance;

class HRuleHandle : public QObject
{
    Q_OBJECT
public:
    explicit HRuleHandle(QObject *parent = 0);
    static HRuleHandle* pInstance;
    static HRuleHandle* Instance();

public:
    //创建规则文件
    bool createFilePath();

    //加载规则文件
    bool loadRuleFile();

    //保存所有规则文件
    bool saveRuleFile();

    //规则文件是否存在
    bool isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType);

    //保存某个厂站下的所有规则
    bool saveStationRuleFile(quint16 wStationNo);

    //导出规则，根据弹出的对话框进行选择
    void exportRuleFiles();

    //删除某个测点的某个类型规则
    void deleteRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType);

    //更改了规则厂站号
    bool changeStationNo(quint16 wOldStationNo,quint16 wNewStationNo);

    int openRuleFrame(
                    QWidget* pWidget, //父窗口
                    quint16 wStationNo, //厂站ID
                    quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                    quint16 wPointNo,  //测点ID
                    quint8  btRelayType, //分，合，检修分，检修合
                    quint16 wRuleID, //规则ID
                    quint8 btType, //规则类型
                    QString &strFormula //公式字符串
                    );

    //初始化规则文件
    bool initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack);

    //退出规则文件
    void exitRuleFiles();
signals:

public slots:
};


/*
bool RULE_EXPORT initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack)
{
    return HRuleHandle::Instance()->initRuleFiles(btType,szFilePath,lpDataCallBack);
}

void RULE_EXPORT exitRuleFiles()
{
    return HRuleHandle::Instance()->exitRuleFiles();
}

int  RULE_EXPORT openRuleFrame(
                    QWidget* pWidget, //父窗口
                    quint16 wStationNo, //厂站ID
                    quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                    quint16 wPointNo,  //测点ID
                    quint8  btRelayType, //分，合，检修分，检修合
                    quint16 wRuleID, //规则ID
                    quint8 btType, //规则类型
                    QString &strFormula //公式字符串
                    )
{
    return HRuleHandle::openRuleFrame(pWidget,wStationNo,wPointType,wPointNo,btRelayType,wRuleID,btType,strFormula);
}
//判断当前测点的某种类型规则是否存在
bool RULE_EXPORT isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType)
{
    return HRuleHandle::Instance()->isRuleFileExist(wStationNo,wPointType,wPointNo,btRelayType);
}

//保存某个厂站下的所有规则
bool RULE_EXPORT saveStationRuleFile(quint16 wStationNo)
{
    return HRuleHandle::Instance()->saveStationRuleFile(wStationNo);
}

//保存所有规则
bool RULE_EXPORT saveAllRuleFile()
{
    return HRuleHandle::Instance()->saveRuleFile();
}


//导出规则，根据弹出的对话框进行选择
void RULE_EXPORT exportRuleFiles()
{
    HRuleHandle::Instance()->exportRuleFiles();
}

//删除某个测点的某个类型规则
void RULE_EXPORT deleteRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType)
{
    HRuleHandle::Instance()->deleteRuleFile(wStationNo,wPointType,wPointNo,btRelayType);
}

//更改了规则厂站号
bool RULE_EXPORT changeStationNo(quint16 wOldStationNo,quint16 wNewStationNo)
{
    return HRuleHandle::changeStationNo(wOldStationNo,wNewStationNo);
}
*/

#endif // HRULEHANDLE_H
