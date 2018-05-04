#ifndef HDRAWOBJ_H
#define HDRAWOBJ_H
#include "ruleexport.h"
#include "frame.h"
#include <QObject>
#include <QDataStream>
#include <QPainter>
#include <QColor>
//#include <QFrame>
#include "ruleeditapi.h"
#define MAXCOUNT_INPUT 30

/*
 * 主要封装绘制对象的属性和操作
*/
class HFrame;
class HRuleFile;

class HDrawObj : public QObject
{
    Q_OBJECT
public:
    //HDrawObj(QObject* object = 0);
    HDrawObj(const QRect& rect,HRuleFile* pRuleFile);
    virtual ~HDrawObj();

public:
    //序列化操作部分
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVersion,QDataStream* ds);
    //属性相关函数
    void generateID();
    void saveOldID();
    quint8 getObjType();
    void setWrongFlag(bool wrong);
    void setVisiteFlag(bool visite);
    void setOffset();

    //绘制操作部分
    virtual void draw(QPainter* painter);
    virtual void drawPins(QPainter* painter,QRect rectPins);
    virtual void drawMain(QPainter* painter,const QRect &rectMain);
    enum SelectState
    {
        normal,
        selected,
        actived
    };
    //绘制左中右四个角的矩形选择框
    virtual void drawSelect(QPainter* painter,SelectState selectState);

    //判断当前鼠标点是否处在左中右的矩形选择框内
    virtual int  hitSelect(const QPoint &point,bool bSelected);

    //选择在移动的时候
    virtual void  moveSelectPoint(int nSelectPoint,HFrame *pFrame,QPoint &point);

    //其他部分
   // virtual void generateID();
    virtual void adjustPosition();
    virtual void calOutPoint(){}//计算对外输出点的位置

    //获取值
    virtual void addInValue(int nNo,bool value);
    virtual bool outValue();//逻辑表达式的值


    QPoint getSelectPoint(int nSelectPoint);
    void getSelectRect(QVector<QRect> &rect);

    //起始符号
    virtual QString getOperatorFirst();
    virtual QString getOperatorMid();
    virtual QString getOperatorLast();

    //补充
public:
    //属性
    quint32 dwOldID;//复制粘贴的时候
    quint32 m_dwID; //图符ID
    quint8 m_btObjType; //图符类型 输入 逻辑等类型
    QString m_strName; //图元名称
    QString m_strRuleName; //规则名称(遥信: ~[xxx站.xx间隔.xx点.工程值])
    bool m_bOutValue;//逻辑输出
    float m_fOutValue;//模拟量输出

    QRect m_rectCurPos; //当前点矩形位置
    HRuleFile* m_pRuleFile; //规则文件
    QPoint* m_pointIn; //多点输入
    QPoint m_pointOut; //实际上对外的连接点位置 对外只有一个输出点
    int m_nInPointSum; //输入点个数
    int m_nOutPointSum; //输出点个数
    bool m_bConnect;

    bool m_bFill; //是否填充
    bool m_bWrong;
    bool m_bVisit;//遍历
    bool m_bInVal[MAXCOUNT_INPUT];//输入对象值

};

//连接线类
/*
 * 方向从右向左看齐
 *  ---                ---
 * |   |/             |   |
 * |In |-out-----In[i]|Out|
 * |   |\             |   |
 *  ---                ---
 */
class  HConnect : public QObject
{
    Q_OBJECT
public:
    enum EDIRECTION{
        enumUp,
        enumDown,
        enumRight,
        enumLeft,
    };

public:
    HConnect();
    HConnect(HRuleFile* pRuleFile);
    //每一条线都有一个输入端和输出端
    HConnect(quint16 dwInObjID,quint16 dwOutObjID,HRuleFile* pRuleFile,quint8 btInIndex);
    ~HConnect();
public:
    //序列化操作
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVerion,QDataStream* ds);

    void setOffSet();
    //绘制操作
    void draw(QPainter* painter);
    void drawSelect(QPainter* painter);
    int  hitSelect(const QPoint &point,bool bSelected);
    void moveTo(int x,int y);
    void  moveSelectPoint(int nSelectPoint,HFrame *pFrame,QPoint &point);
    //void drawArrow(QPainter* painter,QPoint &point,EDIRECTION edirection);
    //删除操作
   // void move(QPoint delta,HGraphicsView* pView);
    //void remove();
    void calLine();
    void calLine(const QPoint& point);
    void setRectABC();
    bool intersects(const QRect &rect);
public:

    QRect rectAB,rectBC,rectCD;//鼠标移动到这三个区域就可以被选中该选择对象
    quint16 m_dwInObjID; //输入元件对象ID
    quint16 m_dwOutObjID;//输出元件对象ID
    quint8 m_btInIndex; //输入元件对象索引
    quint8 m_btOutIndex; //输出元件对象索引
    HRuleFile* m_pRuleFile; //规则文件
    bool m_bConnect;
    QPoint m_pointIn; //输入元件位置
    QPoint m_pointOut; //输出元件位置
    int m_nPageNo; //页面
    QPoint* m_pLinePoint; //所经点

    quint8 m_btPointSum; //点数目
    quint8 m_btSelLine; //选择连接线点号
    QColor m_colorLine; //连接线颜色

};

/* 比较类 遥信比较 遥测比较 */
class  HInputObj:public HDrawObj
{
    Q_OBJECT
public:
    HInputObj(const QRect& rect,HRuleFile* pRuleFile = NULL);
    //virtual ~HInputObj();
public:
    //序列化操作部分
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVerion,QDataStream* ds);
    //绘制操作部分
    virtual void draw(QPainter* painter);
    virtual void calOutPoint();//计算对外输出点的位置

    //获取值
    virtual void addInValue(int nNo,bool value);
    virtual bool outValue();//逻辑表达式的值


    void setInputType(quint8 inputType){btInputType = inputType;}
    void setInputProperty(HFrame* pFrame);
public:
    //属性
    quint8 btInputType;//102:遥信 103:遥测

    //遥信通常是测点的值和0/1进行比较。 遥测通常是遥测1和遥测2比较，遥测1和浮点数比较
    //比较值1：可以是遥信、也可以是遥测
    quint16 m_wMode1;         //间隔or装置
    quint16 m_wStationID1;    //厂站地址
    quint16 m_wProtID1;       //设备地址(读GIN用，用于联锁)  与下面m_btType1二选一
    quint8  m_btType1;        //测点类型(读ID用，大部分用此)
    quint8  m_btInType1;       //内部类型
    quint16 m_wPointID1;      //GIN号/ID号
    quint16 m_wAttr1;         //点属性
    float  fReserved1;       //保留

    //比较值2测点
    quint16 m_wMode2;
    quint16 m_wStationID2;
    quint16 m_wProtID2;
    quint8  m_btType2;
    quint8  m_btInType2;//遥信 遥测 遥控等
    quint16 m_wPointID2;
    quint16 m_wAttr2;
    float fRating;//测点1的额定值 ???
    //比较值2常量
    float m_fCompValue;

    //比较值2的类型
    quint8 m_btCompType;//0:测点比较常数 1:测点比较测点

    //比较值1和比较值2 比较关系
    quint8 m_btCondition;//条件 大于 小于 等于
};

class HResultObj: public HDrawObj
{
    Q_OBJECT
public:
    //HResultObj(QObject* parent = 0);
    HResultObj(const QRect& rect,HRuleFile *pRuleFile);
    //virtual ~HResultObj();

public:
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVerion,QDataStream* ds);
public:
    //绘制操作部分
    virtual void draw(QPainter* painter);

    //获取值
    virtual void addInValue(int nNo,bool value);
    virtual bool outValue();//逻辑表达式的值

    virtual void calOutPoint();//计算对外输出点的位置

public:
    bool bTypeValue;
    float fOutValue;
};

/////////////////////////////////////////////或///////////////////////////////
class HOrObj: public HDrawObj
{
    Q_OBJECT
public:
    //HOrObj(QObject* parent = 0);
    HOrObj(const QRect& rect,HRuleFile *pRuleFile);
    //virtual ~HOrObj();
public:
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVerion,QDataStream* ds);
public:
    //绘制操作部分
    virtual void draw(QPainter* painter);

    //获取值
    virtual void addInValue(int nNo,bool value);
    virtual bool outValue();//逻辑表达式的值
   //virtual void drawPins(QPainter* painter,QRect rectPins);
    //绘制左中右四个角的矩形选择框
    //virtual void drawSelect(QPainter* painter,SelectState selectState);
    //判断当前鼠标点是否处在左中右的矩形选择框内
    //virtual int  hitSelect(const QPoint &point,bool bSelected);
    //选择在移动的时候
    //virtual void  moveSelectPoint(int nSelectPoint,HFrame *pFrame,QPoint &point);
    //其他部分
   // virtual void generateID();
    //virtual void adjustPosition();
    virtual void calOutPoint();//计算对外输出点的位置

};

////////////////////////////////////////////////与//////////////////////////////////
class HAndObj: public HDrawObj
{
    Q_OBJECT
public:
    //HAndObj(QObject* parent = 0);
    HAndObj(const QRect& rect,HRuleFile *pRuleFile);
    //virtual ~HAndObj();
public:
    virtual void readData(int nVersion,QDataStream* ds);
    virtual void writeData(int nVerion,QDataStream* ds);

public:
    //绘制操作部分
    virtual void draw(QPainter* painter);

    //获取值
    virtual void addInValue(int nNo,bool value);
    virtual bool outValue();//逻辑表达式的值
   //virtual void drawPins(QPainter* painter,QRect rectPins);
    //绘制左中右四个角的矩形选择框
    //virtual void drawSelect(QPainter* painter,SelectState selectState);
    //判断当前鼠标点是否处在左中右的矩形选择框内
    //virtual int  hitSelect(const QPoint &point,bool bSelected);
    //选择在移动的时候
    //virtual void  moveSelectPoint(int nSelectPoint,HFrame *pFrame,QPoint &point);
    //其他部分
   // virtual void generateID();
    //virtual void adjustPosition();
    virtual void calOutPoint();//计算对外输出点的位置
};
#endif // HDRAWOBJ_H
