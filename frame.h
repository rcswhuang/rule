#ifndef HFRAME_H
#define HFRAME_H
#include <QFrame>
#include <QList>
#include "rulefile.h"
#include "drawtool.h"
class HFrame : public QFrame
{
public:
    HFrame(QWidget * parent = 0, Qt::WindowFlags f = 0);

public:
    void setDrawShape(DRAWSHAPE drawShape){m_drawShape = drawShape;}
    DRAWSHAPE drawShape(){return m_drawShape;}

public:
    virtual void	keyPressEvent(QKeyEvent * event);
    virtual void	keyReleaseEvent(QKeyEvent * event);
   /* virtual void	leaveEvent(QEvent * event);*/
    virtual void	mouseDoubleClickEvent(QMouseEvent * event);
    virtual void	mouseMoveEvent(QMouseEvent * event);
    virtual void	mousePressEvent(QMouseEvent * event);
    virtual void	mouseReleaseEvent(QMouseEvent * event);
    /*virtual void	moveEvent(QMoveEvent * event);
    virtual bool	nativeEvent(const QByteArray & eventType, void * message, long * result);
    virtual void	paintEvent(QPaintEvent * event);
    virtual void	resizeEvent(QResizeEvent * event);
    virtual void	showEvent(QShowEvent * event);
    virtual void	tabletEvent(QTabletEvent * event);
    virtual void	wheelEvent(QWheelEvent * event);*/
    virtual void	paintEvent(QPaintEvent * event);

public:
    bool isSelect(const QObject* pObj);
    bool select(HDrawObj* pObj,bool bAdd = false);
    bool selectConnect(HConnect* pObj,bool bAdd = false);
    void setSelectCursor(int nSelectPoint);
    void setConnectSelectCursor(int nSelectPoint);
    void drawGrid(QPainter &painter);


    //判断当前对象是否已经连接到连接线,同时要获取连接所在点的编号（对或与等图元来说，是必要的）
    bool isLinkConnectObj(HDrawObj* pObj);
    bool isLinkDrawObj(HConnect* Obj);
    bool findConnectInRect(const HDrawObj*pObj,const quint16 wIndex,int nMoveDirect);
    bool objOutConnectLine(HDrawObj* pObj,const int &deltax,const QPoint &localpoint);
public:
    //QList<HDrawObj*> m_drawObjlist;
    QList<HDrawObj*> m_selectObjList;
   // QList<HConnect*> m_connectObjList;
    QList<HConnect*> m_selectConnectObjList;

    HRuleFile* pRuleFile;//规则文件，QFrame打开的就是一个规则文件
public:
    bool m_bGrid;
private:
    DRAWSHAPE m_drawShape;
};

#endif // HFRAME_H
