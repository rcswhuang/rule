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
    //序列化函数
    friend QDataStream& operator>>(QDataStream &in,HRuleFile&);
    friend QDataStream& operator<<(QDataStream &out,const HRuleFile&);
public:

    //对象操作函数
    quint32 generateDrawObjID()
    {
        return dwDrawObjID++;
    }

    void refreshDrawObjID()
    {
        dwDrawObjID = drawObjList.count()+1;
    }

    void add(HDrawObj* pObj);
    void addConnect(HConnect* pObj);
    HDrawObj* objectAt(const QPoint &point);
    HConnect* connectAt(const QPoint& point);
    HResultObj* resultObj();//查找列表中是否存在输出图元
    HDrawObj* findDrawObj(quint32 ulID);//通过ID来找对象
    void removeObj(HDrawObj* drawObj);
    void removeConnect(HConnect* connectObj);
    bool isObjConnect(HDrawObj* pDrawObj);//判断当前对象是否连接
    HResultObj* getResultObj();
    HDrawObj* getConnectObj(HDrawObj* target,int nConnNo);

    //遍历函数
    bool buildGeneralFormula();
	bool visitGeneralBuildObj(HDrawObj* pObj);

    //仿真遍历
    void buildSimulateFormula();
    void visitSimulateBuildObj(HDrawObj* obj);

    //bool buildEspecialFormula();//遍历规则
   // bool visitEspecialBuildObj(HDrawObj* pDrawObj);//遍历访问某一个图元的所有信息
    //bool visitEspecialReportObj(HDrawObj* pDrawObj,QList<QObject*> pObjList);
signals:

public slots:

public:
    int m_nRuleFileID;//规则文件的ID，每个遥信的每个规则都有ID
    quint32 dwDrawObjID;//每个图元对象的ID,每增加一个图元ID+1
    QString m_strRuleName;//规则名
    QString strFormula;//公式名

    bool m_bGrid;//显示网格
    QString m_strBgClr;
    QString m_strGridClr;
    QString m_strFillClr; //填充色
    QString m_strLineClr; //线条颜色
    //QString m_clrText; //文字颜色
    QString m_strUpedgeClr; //上边框颜色
    QString m_strDownedgeClr; //下边框颜色
    QString m_strShadowClr; //阴影颜色

    bool bDisplayID;//
    bool bSimulateFirst;
    bool bSimuState;
    QSize m_Size;
    bool bFormulaRight;//公式是否正确
    RULEFILEDATA m_ruleFileData;

    QList<HConnect*> connectObjList;
    QList<HDrawObj*> drawObjList;

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
    quint16 m_wStationNo;
    quint16 m_wProtectNo;
    quint16 m_wPointNo;
    quint16 m_wPointType;
    quint8  m_btInsideType;
    quint16 m_wOpenFormulaID;
    quint16 m_wCloseFormulaID;
    quint16 m_wJXOpenFormulaID;
    quint16 m_wJXCloseFormulaID;
    quint16 m_wOpenRuleFileID;
    quint16 m_wCloseRuleFileID;
    quint16 m_wJXOpenRuleFileID;
    quint16 m_wJXCloseRuleFileID;
    QString m_strPointName;
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
    quint16 m_wStationNo;
    quint16 m_wProtectNo;
    QList<HRuleFile*> m_pRuleFileList; //每个点都有一个规则，多少个点就有多少个规则
    QList<HPointRule*> m_pPointRuleList;//每个点都在装置里面
    QString m_strDeviceName;

public:
    bool addPointRule(HPointRule* pPointRule);//增加点规则
    HPointRule* pointRule(quint8 btType,quint16 wPointID);//获取规则
    HPointRule* findPointRule(int nIndex);
    int pointRuleCount(){return m_pPointRuleList.count();}
    bool delPointRuleByID(quint8 btType,int wPointID);

    bool addRuleFile(HRuleFile* pRuleFile);
    HRuleFile* ruleFile(int wType,int wPointNo,int wRelayValue);
    HRuleFile* findRuleFile(int nIndex);
    HRuleFile* getRuleFileByID(quint16 wRuleFileID);
    int ruleFileCount(){return m_pRuleFileList.count();}
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
    quint16 m_wStationNo;
    quint16 m_wRuleFileID;//规则文件的的ID
    QList<HProtectRule*> m_pProtRuleList;
    QString m_strStationName;
    QString m_strRuleFileName;

public:
    QString stationName(){return m_strStationName;}
    QString ruleFileName() {return m_strRuleFileName;}
    quint16 generateID(){return m_wRuleFileID++;}
    void maxRuleFileID();
    bool addProtectRule(HProtectRule* pProtRule);
    HProtectRule* protectRule(quint16 wProtNo);
    HRuleFile* ruleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue);
    bool delRuleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue);
    bool delPointRule(quint16 wType,quint16 wPointNo);

    bool changeStationNo(quint16 wNewStationNo);

    //导出及序列化操作
};


class HStationRuleList
{
public:
    HStationRuleList();
    ~HStationRuleList();
public:
    void addStationRule(HStationRule* stRule);
    HStationRule* findStationRule(wStationID);
protected:
    QList<HStationRule*> m_StationRuleList;
};






















#endif // QRULEFILE_H
