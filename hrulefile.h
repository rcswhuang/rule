#ifndef QRULEFILE_H
#define QRULEFILE_H
#include "hruleexport.h"
#include <QObject>
#include <QList>
#include <QDataStream>
#include "hdrawobj.h"
#include "hruleeditapi.h"
class HConnect;
class HDrawObj;
class HResultObj;
class HRuleFile:public QObject
{
    Q_OBJECT
public:
    explicit HRuleFile(QObject *parent = 0);
    ~HRuleFile();
public:
    //序列化函数
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVersion,QDataStream* ds);
    HRuleFile* clone();
    void copyTo(HRuleFile* file);
public:
    void clear();

    //对象操作函数
    quint16 generateDrawObjID()
    {
        return m_wDrawObjID++;
    }

    void refreshDrawObjID()
    {
        m_wDrawObjID = m_drawObjList.count()+1;
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
    void refreshDrawObj();
    //遍历函数
    bool buildGeneralFormula();
	bool visitGeneralBuildObj(HDrawObj* pObj);

    //仿真遍历
    void buildSimulateFormula();
    void visitSimulateBuildObj(HDrawObj* obj);

    //报告遍历
    void getRuleReport(QString& strRuleReport);
    bool buildReportFormula(QList<QStringList*> *reportList);
    bool visitReportBuildObj(HDrawObj* obj,QList<QStringList*> *reportList);
    void getOrBeforeAnd(QList<QStringList*> *pSrcList,QList<QStringList*> *pDscList,QStringList* pTempList,int i,int total);
signals:

public slots:

public:
    quint16 m_wRuleFileID;//规则文件的ID，每个遥信的每个规则都有ID
    quint16 m_wDrawObjID;//每个图元对象的ID,每增加一个图元ID+1
    QString m_strRuleName;//某个测点规则的名称
    QString m_strFormula;//某个测点的规则（所有图元规则总和,即测点公式）

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
    bool m_bFormulaRight;//公式是否正确
    RULEFILEDATA m_ruleFileData;

    QList<HConnect*> m_connectObjList;
    QList<HDrawObj*> m_drawObjList;

};

class  HPointRule: public QObject
{
    Q_OBJECT
public:
    HPointRule();
    ~HPointRule();

    //序列化操作
public:
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVersion,QDataStream* ds);
public:
    quint16 m_wStationNo;
    quint16 m_wProtectNo;
    quint16 m_wPointNo;
    quint8 m_wPointType;
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
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVersion,QDataStream* ds);
public:
    quint16 m_wStationNo;
    quint16 m_wProtectNo;
    QList<HRuleFile*> m_pRuleFileList; //每个点都有一个规则，多少个点就有多少个规则
    QList<HPointRule*> m_pPointRuleList;//每个点都在装置里面
    QString m_strDeviceName;

public:
    void clear();
    bool addPointRule(HPointRule* pPointRule);//增加点规则
    HPointRule* pointRule(quint8 btType,quint16 wPointID);//获取规则
    HPointRule* findPointRule(int nIndex);
    int pointRuleCount(){return m_pPointRuleList.count();}
    bool delPointRuleByID(quint8 btType,int wPointID);

    bool addRuleFile(HRuleFile* pRuleFile);
    HRuleFile* ruleFile(int wType,int wPointNo,quint8 btYKType);
    HRuleFile* findRuleFile(int nIndex);
    HRuleFile* getRuleFileByID(quint16 wRuleFileID);
    int ruleFileCount(){return m_pRuleFileList.count();}
    bool delRuleFileByID(int wRuleFileID);
    bool delRuleFile(int wType,int wPointNo,quint8 btYKType);


    //相关刷新工作
    void refreshRuleFileID(HPointRule* ptRule);

};

class HStationRule:public QObject
{
    Q_OBJECT
public:
    HStationRule();
    ~HStationRule();
public:
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVersion,QDataStream* ds);
public:
    quint16 m_wStationNo;
    quint16 m_wRuleFileID;//规则文件的的ID
    QList<HProtectRule*> m_pProtRuleList;
    QString m_strStationName;
    QString m_strRuleFileName;//厂站规则文件名称，测试厂站_0.fma

public:
    void clear();
    QString stationName(){return m_strStationName;}
    QString ruleFileName() {return m_strRuleFileName;}
    quint16 generateID(){return m_wRuleFileID++;}
    void maxRuleFileID();
    bool addProtectRule(HProtectRule* pProtRule);
    HProtectRule* protectRule(quint16 wProtNo);
    HRuleFile* ruleFile(quint16 wType,quint16 wPointNo,quint8 btYKType);
    bool delRuleFile(quint16 wType,quint16 wPointNo,quint8 btYKType);
    bool delPointRule(quint16 wType,quint16 wPointNo);

    bool changeStationNo(quint16 wNewStationNo);
    HPointRule* getFirstPointRule();
    QString getRuleFileText();//获取规则文件文本内容
    QString splitRuleText(QString& strRuleText);
    //导出及序列化操作
};


class HStationRuleList
{
public:
    HStationRuleList();
    ~HStationRuleList();
public:
    void clear();
    bool isEmpty(){return m_pStationRuleList.isEmpty();}
    HStationRule* takeFirst()
    {
        return m_pStationRuleList.takeFirst();
    }
    int count(){return m_pStationRuleList.count();}
    void addStationRule(HStationRule* stRule);
    HStationRule* findStationRule(quint16 wStationID);
    HStationRule* at(int index)
    {
        return m_pStationRuleList.at(index);
    }
    void reloadStationRule();

protected:
    QList<HStationRule*> m_pStationRuleList;
};






















#endif // QRULEFILE_H
