#ifndef RULEEDITAPI_H
#define RULEEDITAPI_H
#include <QtGlobal>
#include "ruleexport.h"
class QWidget;
#define MAXSTRINGLEN 256

enum DRAWSHAPE
{
    enumSelection,
    enumConnect,

    //logic
    enumLogicAND,
    enumLogicOR,
    enumLogicNOT,
    enumLogicXOR,
    enumLogicCONST,

    //arithmetic
    enumArithADD,
    enumArithSUB,
    enumArithMUL,
    enumArithDIV,
    enumArithPOW,
    enumArithMAX,
    enumArithMIN,
    enumArithAVE,
    enumArithAVG,
    enumArithSUM,
    enumArithCONST,

    //compare
    enumCompMORE,
    enumCompLESS,
    enumCompEQU,
    enumCompNOEQU,
    enumCompMOREEQU,
    enumCompLESSEQU,

    //mix multiply
    enumMixMULTIPLY,

    //result
    enumInputDIGITAL,
    enumInputCOMP,
    enumInputANALOGUE,
    enumOutRESUTL,
    enumTEXT

};

enum SELECTMODE
{
    enumNone, //空
    enumNetSelect,//这个是矩形框选择
    enumMove,//选择  移动
    enumSize //选择 改变大小
};

//定义规则应用对象，后台，五防本身，装置连锁
//后台，五防而言是测点，装置而言一般是GIN号
#define TYPE_APP_JK   0
#define TYPE_APP_WF   1
#define TYPE_APP_LOCK 2

#define TYPE_RULE_GENERAL  0  //普通规则 适用大部分
#define TYPE_RULE_SECOND   1 //二级规则 暂时不用

//类型定义
#define TYPE_NONE     0
#define TYPE_INPUT    1 //遥信遥测类型
#define TYPE_RESULT   2//输出类型
#define TYPE_LOGICOR  3
#define TYPE_LOGICAND 4

//TYPE_INPUT的子类型
#define TYPE_INPUT_DIGITAL  102
#define TYPE_INPUT_COMP 103


//分 合 检修分 检修合 规则
#define RELAY_OPEN    0x00
#define RELAY_CLOSE   0x01
#define RELAY_JXOPEN  0x02
#define RELAY_JXCLOSE 0x03

#define COND_OPEN  0x00 //遥信分
#define COND_CLOSE 0x01 //遥信合

//定义内部测点类型
#define TYPE_INSID_NULL       0
#define TYPE_INSIDE_ANALOGUE  1
#define TYPE_INSIDE_DIGITAL   2
#define TYPE_INSIDE_SELPOINT  3
#define TYPE_INSIDE_RELAY     4
#define TYPE_INSIDE_PULSE     5
#define TYPE_INSIDE_TUNE      6

//回调函数消息类型
#define WM_SEL_POINT     0  //对话框选择测点
#define WM_ID_GETDBINFO  1  //通过ID来获取测点信息
#define WM_GIN_GETDBINFO 2  //通过GIN来获取测点信息


//比较符定义
#define OP_GREATER      0x01 //>
#define OP_LOWER        0x02  // <
#define OP_EQUAL        0x03 // =
#define OP_GEQUAL       0x04 //>=
#define OP_LEQUAL       0x05 //<=
#define OP_NEQUAL       0x06 // !=

#define TYPE_COMPARE_ANALOGUE 0x01
#define TYPE_COMPARE_CONST    0x02
//规则文件接口类型
/*
 * 主要是接收接口传递过来的站内相关参数信息
*/
typedef struct _tagRuleFileData
{
    QString strStationName;
    QString strProtectName;//装置/间隔
    QString strPointName;
    quint16 wStationNo;
    quint16 wProtectNo;
    quint16 wPointNo;
    quint16 wPointType;//测点类型 遥测 遥信 遥控
    quint16 wFormulaType;//规则类型
    quint16 wFormulaID;//测点分/合等规则ID号
    quint16 wReserve1;
    quint16 wReserve2;
    quint16 wReserve3;
}RULEFILEDATA;

/*
 * 规则模块采用此参数去组态模块或者运行模块获取测点信息
*/
typedef struct _tagRULEPARAM
{
    //
    quint16 wStationNo;//站号
    quint16 wProtectNo; //装置ID 联锁组态用
    quint16 wPointType; //测点类型 五防用
    quint16 wPointNo;//当联锁组态时为GIN号，当是测点类型时为点号
    quint16 wAttr;

    quint8 btInsideType;
    quint16 wOpenFormulaID;
    quint16 wCloseFormulaID;
    quint16 wOpenJXFormulaID;
    quint16 wCloseJXFormulaID;

    QString strStationName;
    QString strProtectName;
    QString strPointName;
    QString strAttr;
    float fRating;//额定值
}RULEPARAM;
//对外接口

/*
 * 规则重要说明：对于后台监控来说，规则是对应到厂站/装置/测点/分、合等等
 * 对于独立五防来说，没有装置一说，测点信息都是全站唯一的。所以获取后台监控的规则和独立五防的规则原理是一致，但做法有点区别
*/



//定义回调函数 去获取实时库的当前测点的信息，包含当前值 当前状态等等
#ifdef __cplusplus
extern "C"
{
#endif
typedef RULE_EXPORT  bool (* LPRULEDATACALLBACK)(int msgType,RULEPARAM *ruleParam);

bool RULE_EXPORT initRuleFiles(quint8 btType,char* szFilePath,LPRULEDATACALLBACK lpDataCallBack);

void RULE_EXPORT exitRuleFiles();

void RULE_EXPORT openRuleWindow();
/*
int  RULE_EXPORT openRuleFrame(
                    QWidget* pWidget, //父窗口
                    quint16 wStationNo, //厂站ID
                    quint16 wPointType, //测点类型 （如果有装置就是装置的地址)
                    quint16 wPointNo,  //测点ID
                    quint8  btRelayType, //分，合，检修分，检修合
                    quint16 wRuleID, //规则ID
                    quint8 btType, //规则类型
                    QString &strFormula //公式字符串
                    );

//判断当前测点的某种类型规则是否存在
bool RULE_EXPORT isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType);

//保存某个厂站下的所有规则
bool RULE_EXPORT saveStationRuleFile(quint16 wStationNo);

//保存所有规则
bool RULE_EXPORT saveAllRuleFile();

//导出规则，根据弹出的对话框进行选择
void RULE_EXPORT exportRuleFiles();

//删除某个测点的某个类型规则
void RULE_EXPORT deleteRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btRelayType);

//更改了规则厂站号
bool RULE_EXPORT changeStationNo(quint16 wOldStationNo,quint16 wNewStationNo);
*/
#ifdef __cplusplus
}
#endif

#endif // RULEEDITAPI_H

