#ifndef HDRAWTOOL_H
#define HDRAWTOOL_H
#include "ruleexport.h"
#include "drawobj.h"
#include "ruleeditapi.h"
#include <QPainter>
#include <QKeyEvent>
class HFrame;
class HDrawObj;
class  HDrawTool
{
public:
    HDrawTool(DRAWSHAPE nDrawShape);

public:
    //on+都是响应鼠标单击事件
    virtual void onMouseDoubleClick(HFrame* pView,const QPoint& point,QMouseEvent* e){}
    virtual void onKeyPress(QKeyEvent *e);
    virtual void onKeyRelease(QKeyEvent *e);
    virtual void onMouseMove(HFrame* pView,const QPoint& point,QMouseEvent* e);
    virtual void onMousePress(HFrame* pView,const QPoint& point,QMouseEvent* e);
    virtual void onMouseRelease(HFrame* pView,const QPoint& point,QMouseEvent* e);
   // virtual void onEditorProperty();

public:
    static HDrawTool* findTool(DRAWSHAPE drawshape);
    static DRAWSHAPE drawShape;
 //   static QList<HDrawTool> m_pDrawToolList;//此列表应该保存在规则文件中，暂时放在此处

public:
    QPoint m_pointDown;//press的点位置，也是移动的点位置开始，随着移动的改变而改变
    //注意还有区域选择的时候点，和上面的点区别
    //QPoint m_pointDown;
    //QPoint m_pointMove;
    bool b_shiftKey;
    bool b_mouseLeftKey;

public:
    DRAWSHAPE m_nDrawShape;
};


class  HSelectTool : public HDrawTool
{
public:
    HSelectTool();

public:
    //on+都是响应鼠标单击事件
    virtual void onMouseDoubleClick(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMouseMove(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMousePress(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMouseRelease(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
   // virtual void onEditorProperty();

public:
    int nDeltaX;//记录当前点到最右边的距离(遥信遥测等单输出对象）
    bool isHaveConnect(HFrame*pFrame,quint16 wIn,quint16 wOut,quint8 btOutInIndex);//是否连接
    void addNewConnect(HFrame*pFrame, HDrawObj* pObj,HDrawObj* pSelectObj,quint8 btIndex);//新的连接
  //  void invalConnect(HGraphicsView* pView,HDrawObj* pObj);//无效连接

    //还有一个connect对象

};

class  HRectTool : public HDrawTool
{
public:
    HRectTool(DRAWSHAPE drawshape);

public:
    //on+都是响应鼠标单击事件
  //  virtual void onMouseDoubleClick(HGraphicsView* pView,const QPoint& point,int nFlags);
    virtual void onMouseMove(HFrame* pView,const QPoint& point,QMouseEvent* e);
    virtual void onMousePress(HFrame* pView,const QPoint& point,QMouseEvent* e);
    virtual void onMouseRelease(HFrame* pView,const QPoint& point,QMouseEvent* e);
};



class  HConnectTool : public HDrawTool
{
public:
    HConnectTool();

public:
    //on+都是响应鼠标单击事件
 //   virtual void onMouseDoubleClick(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMouseMove(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMousePress(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    virtual void onMouseRelease(HFrame* pFrame,const QPoint& point,QMouseEvent* e);
    void draw(QPainter* painter,bool bStartConnect = false,bool bEndConnect = false);
};

#endif // HDRAWTOOL_H
