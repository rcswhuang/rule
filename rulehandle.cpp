#include "rulehandle.h"

QString m_strFilePath ="";
quint8 m_btRuleFile = (quint8)-1;
LPRULEDATACALLBACK m_lpRuleDataCallBack;
HRuleHandle* HRuleHandle::pInstance = NULL;

HRuleHandle::HRuleHandle(QObject *parent) : QObject(parent)
{

}



HRuleHandle* HRuleHandle::Instance()
{
    if(!pInstance)
    {
        pInstance = new HRuleHandle;
    }

    return pInstance;
}

//创建规则文件
bool HRuleHandle::createFilePath()
{

}

//加载规则文件
bool HRuleHandle::loadRuleFile()
{

}

//保存所有规则文件
bool HRuleHandle::saveRuleFile()
{

}

//规则文件是否存在
bool HRuleHandle::isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType)
{

}

//保存某个厂站下的所有规则
bool HRuleHandle::saveStationRuleFile(quint16 wStationNo)
{

}

//导出规则，根据弹出的对话框进行选择
void HRuleHandle::exportRuleFiles()
{

}

//删除某个测点的某个类型规则
void HRuleHandle::deleteRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType)
{

}

//更改了规则厂站号
bool HRuleHandle::changeStationNo(quint16 wOldStationNo,quint16 wNewStationNo)
{

}

int HRuleHandle::openRuleFrame(
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

}

//初始化规则文件
bool HRuleHandle::initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack)
{
    m_btRuleFile = btType;
    if(btType != 1) return false;
    if(lpDataCallBack == NULL) return false;

    m_strFilePath = QString(szFilePath);
    if(!createFilePath()) return false;
    m_lpRuleDataCallBack = lpDataCallBack;

    if(!loadRuleFile()) return false;
    return true;
}

//退出规则文件
void HRuleHandle::exitRuleFiles()
{

}
