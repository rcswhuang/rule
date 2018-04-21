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
    HDrawObj(QObject* object = 0);
    HDrawObj(const QRect& rect,HRuleFile* pRuleFile);


public:
    //序列化操作部分
   // virtual void readData(int nVersion,QDataStream* ds);
   // virtual void writeData(int nVerion,QDataStream* ds);
    //属性相关函数
    quint8 getObjType();
    void setWrongFlag(bool wrong);
    void setVisiteFlag(bool visite);

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
    QPoint getSelectPoint(int nSelectPoint);
    void getSelectRect(QVector<QRect> &rect);

    //起始符号
    virtual QString getOperatorFirst();
    virtual QString getOperatorMid();
    virtual QString getOperatorLast();

    //补充
public:
    //属性
    unsigned long dwID; //图符ID
    quint8 m_btObjType; //图符类型 输入 逻辑等类型
    QString m_strName; //图元名称
    QString m_strRuleName; //规则文件名称
    bool m_bOutValue;//逻辑输出
    float m_fOutValue;//模拟量输出

    QRect m_rectCurPos; //当前点矩形位置
    HRuleFile * m_pRuleFile; //规则文件
    QPoint* m_pointIn; //多点输入
    QPoint m_pointOut; //实际上对外的连接点位置 对外只有一个输出点
    int m_nInPointSum; //输入点个数
    int m_nOutPointSum; //输出点个数
    bool m_bConnect;

    bool m_bFill; //是否填充
    QColor m_clrFill; //填充色
    QColor m_clrLine; //线条颜色
    QColor m_clrText; //文字颜色
    QColor m_clrUpedge; //上边框颜色
    QColor m_clrDownedge; //下边框颜色
    QColor m_clrShadow; //阴影颜色

    int nLeftDeltaX;//输入点在左边
    int nRightDeltaX;//输出点在右边

    bool m_bWrong;
    bool m_bVisit;//遍历

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

public:
    //序列化操作
    //virtual void readData(int nVersion,QDataStream* ds);
    //virtual void writeData(int nVerion,QDataStream* ds);
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
    quint16 dwInObjID; //输入元件对象ID
    quint16 dwOutObjID;//输出元件对象ID
    quint8 btInIndex; //输入元件对象索引
    quint8 btOutIndex; //输出元件对象索引
    HRuleFile* m_pRuleFile; //规则文件
    bool bConnect;
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

public:
    //序列化操作部分
  //  virtual void readData(int nVersion,QDataStream* ds);
   // virtual void writeData(int nVerion,QDataStream* ds);

    //绘制操作部分
    virtual void draw(QPainter* painter);

    virtual void calOutPoint();//计算对外输出点的位置
    void setInputType(quint8 inputType){btInputType = inputType;}
    void setInputProperty(HFrame* pFrame);
public:
    //属性
    quint8 btInputType;//102:遥信 103:遥测

    //遥信通常是测点的值和0/1进行比较。 遥测通常是遥测1和遥测2比较，遥测1和浮点数比较
    //比较值1：可以是遥信、也可以是遥测
    quint16 wMode1;         //间隔or装置
    quint16 wStationID1;    //厂站地址
    quint16 wProtID1;       //设备地址
    quint8 btInType1;       //遥信 遥测 遥控等
    quint16 wPointID1;      //点号
    quint16 wAttr1;         //点属性
    float fReserved1;       //保留

    //比较值2
    quint16 wMode2;
    quint16 wStationID2;
    quint16 wProtID2;
    quint8 btInType2;//遥信 遥测 遥控等
    quint16 wPointID2;
    quint16 wAttr2;
    float fRating;//测点1的额定值 ???

    //比较值2
    float m_fCompValue;

    bool bCompType;//0:测点比较常数 1:测点比较测点
    quint8 btCondition;//条件 大于 小于 等于

    //输出
    float m_fOutValue;
    bool m_bOutValue;
};

class HResultObj: public HDrawObj
{
    Q_OBJECT
public:
    HResultObj(QObject* parent = 0);
    HResultObj(const QRect& rect,HRuleFile *pRuleFile);
public:
    //绘制操作部分
    virtual void draw(QPainter* painter);
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

public:
    bool bTypeValue;
    float fOutValue;
};

/////////////////////////////////////////////或///////////////////////////////
class HOrObj: public HDrawObj
{
    Q_OBJECT
public:
    HOrObj(QObject* parent = 0);
    HOrObj(const QRect& rect,HRuleFile *pRuleFile);
public:
    //绘制操作部分
    virtual void draw(QPainter* painter);
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
public:
    bool outValue(){return false;}//逻辑表达式的值
public:
    void reCalOrInputPoint();//重新计算输入点个数的坐标，或可能是多个元素一起或
};

////////////////////////////////////////////////与//////////////////////////////////
class HAndObj: public HDrawObj
{
    Q_OBJECT
public:
    HAndObj(QObject* parent = 0);
    HAndObj(const QRect& rect,HRuleFile *pRuleFile);
public:
    //绘制操作部分
    virtual void draw(QPainter* painter);
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
public:
    bool outValue(){return false;}//逻辑表达式的值
public:
    void reCalOrInputPoint();//重新计算输入点个数的坐标，或可能是多个元素一起或
};





#endif // HDRAWOBJ_H
