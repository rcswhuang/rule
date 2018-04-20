#ifndef QRULEFILE_H
#define QRULEFILE_H
#include "ruleexport.h"
#include <QObject>
#include <QList>
#include <QDataStream>
#include "drawobj.h"
#include "ruleeditapi.h"
class HConnect;
class HDrawObj;
class HResultObj;
class HRuleFile:public QObject
{
    Q_OBJECT
public:
    explicit HRuleFile(QObject *parent = 0);

public:
    int nRuleFileID;//规则文件的ID，每个遥信的每个规则都有ID
    unsigned long dwDrawObjID;//每个图元对象的ID,每增加一个图元ID+1
    QString strRuleName;//规则名
    QString strFormula;//公式名

    bool bGrid;//显示网格
    bool bDisplayID;//
    bool bSimulateFirst;
    bool bSimuState;
    bool bFromulaRight;

    QColor gridColor;

    QList<HConnect*> connectObjList;
    QList<HDrawObj*> drawObjList;

public:
    //序列化函数
    friend QDataStream& operator>>(QDataStream &in,HRuleFile&);
    friend QDataStream& operator<<(QDataStream &out,const HRuleFile&);
public:
    unsigned long generateDrawObjID()
    {
        return ++dwDrawObjID;
    }
    void add(HDrawObj* pObj);
    void addConnect(HConnect* pObj);
    HDrawObj* objectAt(const QPoint &point);
    HConnect* connectAt(const QPoint& point);
    HResultObj* resultObj();//查找列表中是否存在输出图元
    HDrawObj* findDrawObj(unsigned long ulID);//通过ID来找对象


    void removeObj(HDrawObj* drawObj);
    void removeConnect(HConnect* connectObj);
    bool isObjConnect(HDrawObj* pDrawObj);//判断当前对象是否连接

    //遍历函数
    //bool buildEspecialFormula();//遍历规则
   // bool visitEspecialBuildObj(HDrawObj* pDrawObj);//遍历访问某一个图元的所有信息
    //bool visitEspecialReportObj(HDrawObj* pDrawObj,QList<QObject*> pObjList);
signals:

public slots:

};

class  HPointRule: public QObject
{
    Q_OBJECT
public:
    HPointRule();
    ~HPointRule();

    //序列化操作
public:
    friend  QDataStream& operator>>(QDataStream& in,HPointRule&);
    friend  QDataStream& operator<<(QDataStream& out,const HPointRule&);
public:
    quint16 wStationNo;
    quint16 wProtectNo;
    quint16 wPointNo;
    quint16 wPointType;
    quint8  btInsideType;
    quint16 wOpenFormulaID;
    quint16 wCloseFormulaID;
    quint16 wJXOpenFormulaID;
    quint16 wJXCloseFormulaID;
    quint16 wOpenRuleFileID;
    quint16 wCloseRuleFileID;
    quint16 wJXOpenRuleFileID;
    quint16 wJXCloseRuleFileID;
    char szPointName[MAXSTRINGLEN];
};


class  HProtectRule:public QObject
{
    Q_OBJECT

public:
    HProtectRule();
    ~HProtectRule();
public:
    friend QDataStream& operator>>(QDataStream &in,HProtectRule&);
    friend QDataStream& operator<<(QDataStream &out,const HProtectRule&);
public:
    quint16 wStationNo;
    quint16 wProtectNo;
    QList<HRuleFile*> pRuleFileList; //每个点都有一个规则，多少个点就有多少个规则
    QList<HPointRule*> pPointRuleList;//每个点都在装置里面
    char szDeviceName[MAXSTRINGLEN];

public:
    bool addPointRule(HPointRule* pPointRule);//增加点规则
    HPointRule* pointRule(quint8 btType,quint16 wPointID);//获取规则
    HPointRule* findPointRule(int nIndex);
    int pointRuleCount(){return pPointRuleList.count();}
    bool delPointRuleByID(quint8 btType,int wPointID);

    bool addRuleFile(HRuleFile* pRuleFile);
    HRuleFile* ruleFile(int wType,int wPointNo,int wRelayValue);
    HRuleFile* findRuleFile(int nIndex);
    HRuleFile* getRuleFileByID(quint16 wRuleFileID);
    int ruleFileCount(){return pRuleFileList.count();}
    bool delRuleFileByID(int wRuleFileID);
    bool delRuleFile(int wType,int wPointNo,int wReleayNo);

    //序列化操作

    //相关刷新工作

};

class HStationRule:public QObject
{
    Q_OBJECT
public:
    HStationRule();
    ~HStationRule();
public:
    friend QDataStream& operator>>(QDataStream& in,HStationRule&);
    friend QDataStream& operator<<(QDataStream& out,const HStationRule&);
public:
    quint16 wStationNo;
    quint16 wRuleFileID;//规则文件的的ID
    QList<HProtectRule*> pProtRuleList;
    char szStationName[MAXSTRINGLEN];
    char szRuleFileName[MAXSTRINGLEN];

public:
    char* stationName(){return szStationName;}
    char* ruleFileName() {return szRuleFileName;}
    quint16 generateID(){return wRuleFileID++;}
    void maxRuleFileID();
    bool addProtectRule(HProtectRule* pProtRule);
    HProtectRule* protectRule(quint16 wProtNo);
    HRuleFile* ruleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue);
    bool delRuleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue);
    bool delPointRule(quint16 wType,quint16 wPointNo);

    bool changeStationNo(quint16 wNewStationNo);

    //导出及序列化操作
};























#endif // QRULEFILE_H
