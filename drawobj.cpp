#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "drawobj.h"
#include <QPen>
#include <QPoint>
#include <QBrush>
//#include "frame.h"
#include "hdigitalprop.h"
#include "rulefile.h"
#include <math.h>
#include <QDebug>
/*
HDrawObj::HDrawObj(QObject *parent) : QObject(parent)
{

}
*/
HDrawObj::HDrawObj(const QRect &rect, HRuleFile *pRuleFile)
    :m_rectCurPos(rect),m_pRuleFile(pRuleFile)
{
    m_bConnect = false;
    m_btObjType = TYPE_NONE; //图符类型
    m_strName=""; //图元名称
    m_strRuleName = ""; //规则文件名称
    m_bOutValue = false;//逻辑输出
    m_fOutValue = 0.0;//模拟量输出
    m_pointIn = NULL; //多点输入
    //m_pointOut; //实际上对外的连接点位置 对外只有一个输出点
    m_nInPointSum = 0; //输入点个数
    m_nOutPointSum = 1; //输出点个数
    m_bFill = true; //是否填充
    if(m_pRuleFile)
    {
        m_dwID = m_pRuleFile->generateDrawObjID();
        qDebug()<<"dwID = "<<m_dwID;
    }
}

HDrawObj::~HDrawObj()
{
    if(m_pointIn) //多点输入
    {
        delete[] m_pointIn;
        m_pointIn = NULL;
    }
}

void HDrawObj::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;

    quint32 n32;
    *ds >> n32; //图符ID
    m_dwID = n32;
    quint8  n8;
    *ds >> n8;
    m_btObjType = n8; //图符类型 输入 逻辑等类型
    QString s;
    *ds >> s;
    m_strName = s; //图元名称
    *ds >> s;
    m_strRuleName = s; //规则文件名称
    QRect rect;
    *ds >> rect;
    m_rectCurPos = rect;//当前点矩形位置
    int n;
    *ds >> n;
    m_nInPointSum = n; //输入点个数
    QPoint pt;
    if(m_pointIn)
    {
        delete[] m_pointIn;
        m_pointIn = NULL;
    }
    if(m_nInPointSum == 0)
        m_pointIn = NULL;
    else
    {
        m_pointIn = new QPoint[m_nInPointSum];
        for(int i = 0; i < m_nInPointSum;i++)
        {
            *ds >> pt;
            m_pointIn[i] = pt;
        }
    }

    *ds >> n;
    m_nOutPointSum = n; //输出点个数
    *ds >> pt;
    m_pointOut = pt;

}

void HDrawObj::writeData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return ;
    *ds<<(quint32)m_dwID; //图符ID
    *ds<<(quint8)m_btObjType; //图符类型 输入 逻辑等类型
    *ds<<(QString)m_strName; //图元名称
    *ds<<(QString)m_strRuleName; //规则文件名称
    *ds<<(QRect)m_rectCurPos; //当前点矩形位置
    *ds<<(int)m_nInPointSum;
    if(m_pointIn)
    {
        for(int i = 0; i < m_nInPointSum;i++)
            *ds<<(QPoint)m_pointIn[i];
    }
    *ds<<(int)m_nOutPointSum;
    *ds<<(QPoint)m_pointOut;
}

void HDrawObj::generateID()
{
    if(m_pRuleFile)
    {

        m_dwID = m_pRuleFile->generateDrawObjID();
        qDebug()<<"m_dwID = "<<m_dwID;
    }
}

void HDrawObj::saveOldID()
{
    dwOldID = m_dwID;
}

void HDrawObj::setOffset()
{
    m_rectCurPos.adjust(10,10,10,10);
}

quint8 HDrawObj::getObjType()
{
    return m_btObjType;
}

void HDrawObj::setWrongFlag(bool wrong)
{
    m_bWrong = wrong;
}

void HDrawObj::setVisiteFlag(bool visite)
{
    m_bVisit = visite;
}

void HDrawObj::draw(QPainter *painter)
{
    adjustPosition();
}

void HDrawObj::adjustPosition()
{
    //主要是判断创建图元时，图元是否创建在边界附近
    QRect maxRect(0,0,0,0);
    
    //超出上部边界
    if(m_rectCurPos.top() < maxRect.top())
    {
       // m_rectCurPos = m_rectCurPos + QPoint(0,maxRect.top() - m_rectCurPos.top());
    }
    
    //超出底部边界
    if(m_rectCurPos.bottom() > maxRect.bottom())
    {
        //m_rectCurPos = m_rectCurPos + QPoint(0,maxRect.bottom() - m_rectCurPos.bottom()); //负值
    }
    
    //超出左边界
    if(m_rectCurPos.left() < maxRect.left())
    {
        //m_rectCurPos = m_rectCurPos + QPoint(maxRect.left() - m_rectCurPos.left(),0);
    }
    
    //超过右边界
    if(m_rectCurPos.right() > maxRect.right())
    {
        //m_rectCurPos = m_rectCurPos + QPoint(maxRect.right() - m_rectCurPos.right(),0);
    }
    
}

void HDrawObj::drawMain(QPainter *painter, const QRect &rectMain)
{
    if(painter == NULL) return;
    painter->save();
    QBrush brush;
    brush.setColor(QColor(m_pRuleFile->m_strFillClr));
    brush.setStyle(Qt::SolidPattern);
    QPen pen;
    pen.setColor(QColor(m_pRuleFile->m_strUpedgeClr));
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    //pen.setBrush(brush);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(rectMain);

    //边缘
    QPen pen2;
    pen2.setColor(QColor(m_pRuleFile->m_strDownedgeClr));
    pen2.setStyle(Qt::SolidLine);
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawLine(QPoint(rectMain.left()+2,rectMain.bottom()),QPoint(rectMain.right(),rectMain.bottom()));
    painter->drawLine(QPoint(rectMain.right(),rectMain.bottom()),QPoint(rectMain.right(),rectMain.top()-2));

    //阴影
    QPen pen3;
    pen3.setColor(QColor(m_pRuleFile->m_strShadowClr));
    pen3.setStyle(Qt::SolidLine);
    pen3.setWidth(2);
    painter->setPen(pen3);
    painter->drawLine(QPoint(rectMain.left()+1,rectMain.bottom()+1),QPoint(rectMain.right()-1,rectMain.bottom()+1));
    painter->drawLine(QPoint(rectMain.right()-1,rectMain.bottom()+1),QPoint(rectMain.right()-1,rectMain.top()-1));

    //线条
    QPen pen4;
    pen4.setColor(QColor(m_pRuleFile->m_strDownedgeClr));
    pen4.setStyle(Qt::SolidLine);
    pen4.setWidth(1);
    painter->setPen(pen4);
    painter->drawLine(QPoint(rectMain.left()+2,rectMain.top() + rectMain.height()/(m_nInPointSum+2)),QPoint(rectMain.right()-2,rectMain.top() + rectMain.height()/(m_nInPointSum + 2)));
    QPen pen5;
    pen5.setColor(QColor(m_pRuleFile->m_strShadowClr));
    pen5.setStyle(Qt::SolidLine);
    pen5.setWidth(1);
    painter->setPen(pen5);
    painter->drawLine(QPoint(rectMain.left()+2,rectMain.top() + rectMain.height()/(m_nInPointSum+2)+1),QPoint(rectMain.right()-2,rectMain.top() + rectMain.height()/(m_nInPointSum + 2)+1));
    painter->restore();
}

void HDrawObj::drawPins(QPainter *painter, QRect rectPins)
{
    painter->save();
    QPoint pt[7];

    //输入点 多几个针脚而已
    for(int i = 0; i < m_nInPointSum;i++)
    {
        pt[0].setX(rectPins.left());
        pt[0].setY(m_pointIn[i].y());
        pt[1].setX(m_pointIn[i].x() + (rectPins.left() - m_pointIn[i].x())/2);
        pt[1].setY(m_pointIn[i].y());
        pt[2].setX(pt[1].x() - 2);
        pt[2].setY(m_pointIn[i].y() - 3);
        pt[3].setX(m_pointIn[i].x());
        pt[3].setY(m_pointIn[i].y() - 3);
        pt[4].setX(m_pointIn[i].x());
        pt[4].setY(m_pointIn[i].y() + 3);
        pt[5].setX(pt[1].x() - 2);
        pt[5].setY(m_pointIn[i].y() + 3);
        pt[6] = pt[1];
        painter->drawPolygon(pt,7);
    }

    //输出点 一个针脚
    for(int i = 0; i < m_nOutPointSum; i++)
    {
        pt[0].setX(rectPins.right());
        pt[0].setY(m_pointOut.y());
        pt[1].setX(m_pointOut.x() - (m_pointOut.x() - rectPins.right())/2);
        pt[1].setY(m_pointOut.y());
        pt[2].setX(pt[1].x() + 2);
        pt[2].setY(m_pointOut.y() - 3);
        pt[3].setX(m_pointOut.x());
        pt[3].setY(m_pointOut.y() - 3);
        pt[4].setX(m_pointOut.x());
        pt[4].setY(m_pointOut.y() + 3);
        pt[5].setX(pt[1].x() + 2);
        pt[5].setY(m_pointOut.y() + 3);
        pt[6] = pt[1];
        painter->drawPolygon(pt,7);
    }
    painter->restore();
}

QPoint HDrawObj::getSelectPoint(int nSelectPoint)
{
    int x,y,xCenter,yCenter;
    xCenter = m_rectCurPos.left() + m_rectCurPos.width()/2;
    yCenter = m_rectCurPos.top() + m_rectCurPos.height()/2;
    switch(nSelectPoint)
    {
    case 1:
        x = m_rectCurPos.left();
        y = m_rectCurPos.top();
        break;
    case 2:
        x = xCenter;
        y = m_rectCurPos.top();
        break;
    case 3:
        x = m_rectCurPos.right();
        y = m_rectCurPos.top();
        break;
    case 4:
        x = m_rectCurPos.right();
        y = yCenter;
        break;
    case 5:
        x = m_rectCurPos.right();
        y = m_rectCurPos.bottom();
        break;
    case 6:
        x = xCenter;
        y = m_rectCurPos.bottom();
        break;
    case 7:
        x = m_rectCurPos.left();
        y = m_rectCurPos.bottom();
        break;
    case 8:
        x = m_rectCurPos.left();
        y = yCenter;
        break;
    }
    return QPoint(x,y);
}

void HDrawObj::getSelectRect(QVector<QRect> &rect)
{
    int x,y,xCenter,yCenter;
    xCenter = m_rectCurPos.left() + m_rectCurPos.width()/2;
    yCenter = m_rectCurPos.top() + m_rectCurPos.height()/2;

    x = m_rectCurPos.left();
    y = m_rectCurPos.top();
    rect.append(QRect(x-3,y-3,7,7));

    x = xCenter;
    y = m_rectCurPos.top();
    rect.append(QRect(x-3,y-3,7,7));

    x = m_rectCurPos.right();
    y = m_rectCurPos.top();
    rect.append(QRect(x-3,y-3,7,7));

    x = m_rectCurPos.right();
    y = yCenter;
    rect.append(QRect(x-3,y-3,7,7));

    x = m_rectCurPos.right();
    y = m_rectCurPos.bottom();
    rect.append(QRect(x-3,y-3,7,7));

    x = xCenter;
    y = m_rectCurPos.bottom();
    rect.append(QRect(x-3,y-3,7,7));

    x = m_rectCurPos.left();
    y = m_rectCurPos.bottom();
    rect.append(QRect(x-3,y-3,7,7));

    x = m_rectCurPos.left();
    y = yCenter;
    rect.append(QRect(x-3,y-3,7,7));
}

void HDrawObj::drawSelect(QPainter *painter, SelectState selectState)
{
    switch(selectState)
    {
    case normal:
        break;
    case selected:
    case actived:
    {
        /*for(int i = 0; i < 8; i++)
        {
            QPoint point = getSelectRect(i);
            painter->drawRect(point.x()-3,point.y()-3,7,7);
        }*/
        QVector<QRect> rectList;
        getSelectRect(rectList);
        painter->drawRects(rectList);
    }
        break;
    }
}

int HDrawObj::hitSelect(const QPoint &point, bool bSelected)
{
    if(bSelected)
    {
        for(int nSelectPoint = 1; nSelectPoint <= 8; nSelectPoint++)
        {
            QPoint iPoint = getSelectPoint(nSelectPoint);
            //QRect rectPoint(QPoint(iPoint.x()-3,iPoint.y()-3),iPoint);
            QRect rectPoint(iPoint.x()-3,iPoint.y()-3,10,10);
            if(point.x() >= rectPoint.left() && point.x() <= rectPoint.right() && point.y() >= rectPoint.top() && point.y() <= rectPoint.bottom())
                return nSelectPoint;
        }
    }
    return 0;
}

void HDrawObj::moveSelectPoint(int nSelectPoint, HFrame *pFrame, QPoint &point)
{
    QPoint selectPoint = getSelectPoint(nSelectPoint);
    if(nSelectPoint == 1)//左上
    {
        m_rectCurPos.setLeft(point.x());
        m_rectCurPos.setTop(point.y());
    }
    else if(nSelectPoint == 3)//右上
    {
        m_rectCurPos.setRight(point.x());
        m_rectCurPos.setTop(point.y());
    }
    else if(nSelectPoint == 4)//右中位置
    {
        m_rectCurPos.setRight(point.x());
    }
    else if(nSelectPoint == 5) //右下
    {
        m_rectCurPos.setRight(point.x());
        m_rectCurPos.setBottom(point.y());
    }
    else if(nSelectPoint == 7) //左下
    {
        m_rectCurPos.setLeft(point.x());
        m_rectCurPos.setBottom(point.y());
    }
    else if(nSelectPoint == 8 )//左中位置
    {
        m_rectCurPos.setLeft(point.x());
    }
   // pFrame->update();
}

QString HDrawObj::getOperatorFirst()
{
    QString strOperator;
    switch(m_btObjType)
    {
    default:
        return "";
    case TYPE_INPUT:
        strOperator = " ";
        break;
    case TYPE_LOGICAND:
    case TYPE_LOGICOR:
        strOperator = " (";
        break;
    }
    return strOperator;
}

QString HDrawObj::getOperatorMid()
{
    QString strOperator;
    switch(m_btObjType)
    {
    default:
        return "";
    case TYPE_LOGICAND:
        strOperator = " & ";
        break;
    case TYPE_LOGICOR:
        strOperator = " | ";
        break;
    case TYPE_INPUT:
        strOperator = m_strRuleName;
        break;
    }
    return strOperator;
}

QString HDrawObj::getOperatorLast()
{
    QString strOperator;
    switch(m_btObjType)
    {
    default:
        return "";
    case TYPE_LOGICAND:
    case TYPE_LOGICOR:
        strOperator = " )";
        break;
    case TYPE_INPUT:
        strOperator = " ";
        break;
    }
    return strOperator;
}

void HDrawObj::addInValue(int nNo,bool value)
{
    memset(&m_bInVal,0,MAXCOUNT_INPUT);
}

bool HDrawObj::outValue()//逻辑表达式的值
{
    return m_bOutValue;
}
////////////////////////////////////////HConnect///////////////////////////////////////
HConnect::HConnect()
{
    m_pLinePoint = NULL;
    m_pRuleFile = NULL;
    btOutIndex = (quint8)-1;
    btInIndex = (quint8)-1;
    dwInObjID = (quint16)-1;
    dwOutObjID =(quint16)-1;
}

HConnect::HConnect(HRuleFile *pRuleFile)
{
    m_pRuleFile = pRuleFile;
    m_pLinePoint = NULL;
    m_btSelLine = 0;
    btOutIndex = (quint8)-1;
    btInIndex = (quint8)-1;
    dwInObjID = (quint16)-1;
    dwOutObjID =(quint16)-1;
}

HConnect::HConnect(quint16 dwInObjID, quint16 dwOutObjID, HRuleFile *pRuleFile, quint8 btInIndex)
{
    this->dwInObjID = dwInObjID;
    this->dwOutObjID = dwOutObjID;
    this->m_pRuleFile = pRuleFile;
    HDrawObj* pObjIn = pRuleFile->findDrawObj(dwInObjID);
    HDrawObj* pObjOut = pRuleFile->findDrawObj(dwOutObjID);
    m_pointIn = pObjIn->m_pointOut;
    m_pointOut = pObjOut->m_pointIn[btInIndex];
    btOutIndex = btInIndex;
    m_btSelLine = 0;
    m_pLinePoint = NULL;

    calLine();
}

HConnect::~HConnect()
{
    if(m_pLinePoint){
        delete[] m_pLinePoint;
        m_pLinePoint = NULL;
    }
}

void HConnect::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    quint16 n16;
    *ds>>n16;
    dwInObjID = n16;
    *ds>>n16;
    dwOutObjID = n16;
    quint8 n8;
    *ds>>n8;
    btInIndex = n8;
    *ds>>n8;
    btOutIndex = n8;
    QPoint pt;
    *ds>>pt;
    m_pointIn = pt;
    *ds>>pt;
    m_pointOut = pt;
    *ds>>n8;
    m_btPointSum = n8;
    if(m_btPointSum == 0) m_pLinePoint = NULL;
    else
    {
        m_pLinePoint = new QPoint[m_btPointSum];
        for(int i = 0; i < m_btPointSum;i++)
        {
            *ds>>pt;
            m_pLinePoint[i] = pt;
        }
    }
    *ds>>n8;
    m_btSelLine = n8;
}

void HConnect::writeData(int nVerion,QDataStream* ds)
{
    if(!ds) return;
    *ds<<(quint16) dwInObjID; //输入元件对象ID
    *ds<<(quint16) dwOutObjID;//输出元件对象ID
    *ds<<(quint8) btInIndex; //输入元件对象索引
    *ds<<(quint8) btOutIndex; //输出元件对象索引
    *ds<<(QPoint) m_pointIn; //输入元件位置
    *ds<<(QPoint) m_pointOut; //输出元件位置
    *ds<<(quint8) m_btPointSum; //点数目
    if(m_btPointSum > 0)
    {
        for(int i = 0; i < m_btPointSum;i++)
            *ds<<(QPoint)m_pLinePoint[i];
    }
    *ds<<(quint8)m_btSelLine; //选择连接线点号
}

void HConnect::setOffSet()
{
    m_pointIn += QPoint(10,10);
    m_pointOut += QPoint(10,10);
    calLine();
}

//连接的线型：2中
/*
 * 1.   A点 口----------口 B点 平行或者垂直  经过的点2个 就是A和B两点
 *
 *
 * 2.     C点 口-------口 D点
 *            |
 * A点 口-----口 B点  经过的点4个 A、B、C、D4个点
 */

/*
 * 计算连接线绘制路线
*/
void HConnect::calLine()
{
    QPoint pointIn = m_pointIn;
    QPoint pointOut = m_pointOut;
    if(m_pLinePoint != NULL)
    {
        delete[] m_pLinePoint;
        m_pLinePoint = NULL;
    }

    if(pointIn.x() == pointOut.x() || pointIn.y() == pointOut.y()) //垂直或者水平
    {
        m_pLinePoint = new QPoint[2];
        m_pLinePoint[0] = QPoint(pointIn.x()-1,pointIn.y());
        m_pLinePoint[1] = pointOut;
        m_btPointSum = 2;
    }
    else if(pointIn.x() < pointOut.x())//A和D点
    {
        m_pLinePoint = new QPoint[4];
        m_pLinePoint[0] = QPoint(pointIn.x()-1,pointIn.y());//A点
        m_pLinePoint[1] = QPoint(pointIn.x()+(pointOut.x()-pointIn.x())/2,pointIn.y());//B点
        m_pLinePoint[2] = QPoint(pointIn.x()+(pointOut.x()-pointIn.x())/2,pointOut.y());//C点
        m_pLinePoint[3] = pointOut;//D点
        m_btPointSum = 4;
     }
     else if(pointIn.x() > pointOut.x() && pointIn.y() < pointOut.y())
    {
        m_pLinePoint = new QPoint[6];
        m_pLinePoint[0] = QPoint(pointIn.x()-1,pointIn.y());
        m_pLinePoint[1] = QPoint(pointIn.x()+10,pointIn.y());
        m_pLinePoint[2] = QPoint(pointIn.x()+10,pointIn.y() - 5*10);
        m_pLinePoint[3] = QPoint(pointOut.x()-10,pointIn.y()-5*10);
        m_pLinePoint[4] = QPoint(pointOut.x()-10,pointOut.y());
        m_pLinePoint[5] = pointOut;
        m_btPointSum = 6;
    }
    else if(pointIn.x() > pointOut.x() && pointIn.y() > pointOut.y())
    {
        m_pLinePoint = new QPoint[6];
        m_pLinePoint[0] = QPoint(pointIn.x()-1,pointIn.y());
        m_pLinePoint[1] = QPoint(pointIn.x()+10,pointIn.y());
        m_pLinePoint[2] = QPoint(pointIn.x()+10,pointIn.y() + 5*10);
        m_pLinePoint[3] = QPoint(pointOut.x()-10,pointIn.y()+ 5*10);
        m_pLinePoint[4] = QPoint(pointOut.x()-10,pointOut.y());
        m_pLinePoint[5] = pointOut;
        m_btPointSum = 6;
    }
}

void HConnect::calLine(const QPoint &point)
{
    if(!m_pLinePoint)
    {
        delete[] m_pLinePoint;
        m_pLinePoint = NULL;
    }
    QPoint midPoint = point;
    m_pLinePoint = new QPoint[4];
    m_pLinePoint[0] = QPoint(midPoint.x()-60,midPoint.y()+15);
    m_pLinePoint[1] = QPoint(midPoint.x(),midPoint.y()+15);
    m_pLinePoint[2] = QPoint(midPoint.x(),midPoint.y()-15);
    m_pLinePoint[3] = QPoint(midPoint.x()+60,midPoint.y()-15);
    m_btPointSum = 4;
    m_pointIn = m_pLinePoint[0];
    m_pointOut = m_pLinePoint[4];
}


void HConnect::moveTo(int deltaX, int deltaY)
{
    if(m_pLinePoint == NULL)
        return;
    m_pLinePoint[1] = QPoint(m_pLinePoint[1].x() + deltaX,m_pLinePoint[1].y() + deltaY);//B点
    m_pLinePoint[2] = QPoint(m_pLinePoint[2].x() + deltaX,m_pLinePoint[2].y() + deltaY);//c点
    m_pLinePoint[0] = QPoint(m_pLinePoint[0].x() + deltaX,m_pLinePoint[0].y() + deltaY);//A点
    m_pLinePoint[3] = QPoint(m_pLinePoint[3].x() + deltaX,m_pLinePoint[3].y() + deltaY);//D点
    m_btPointSum = 4;

    setRectABC();
}

void HConnect::setRectABC()
{
    if(m_pLinePoint == NULL)
        return;
    rectAB.setRect(m_pLinePoint[0].x() - 5 ,m_pLinePoint[0].y()-5,m_pLinePoint[1].x() - m_pLinePoint[0].x(),11);
    rectBC.setRect(m_pLinePoint[1].x() - 5,m_pLinePoint[1].y(),11,(m_pLinePoint[2].y() - m_pLinePoint[1].y()));
    rectCD.setRect(m_pLinePoint[2].x() - 5,m_pLinePoint[2].y()- 5,m_pLinePoint[3].x() - m_pLinePoint[2].x(),55);
}

bool HConnect::intersects(const QRect &rect)
{
    if(m_pLinePoint == NULL) return false;
    if(rectAB.intersects(rect) || rectBC.intersects(rect) || rectCD.intersects(rect))
        return true;
    return false;
}

/*
 * 绘制线条部分
*/
void HConnect::draw(QPainter *painter)
{
    painter->save();
    QPen pen(Qt::SolidLine);
    pen.setWidth(2);
    if(!m_btSelLine)
        pen.setColor(QColor(20,180,50));
    else
        pen.setColor(QColor(255,0,255));
    painter->setPen(pen);
    painter->drawPolyline(m_pLinePoint,m_btPointSum);
    painter->restore();
}

void HConnect::drawSelect(QPainter *painter)
{
    if(m_pLinePoint == NULL) return;
    painter->save();
    QBrush brush(QColor(0,0,0));
    painter->setBrush(brush);
    QRect rectA(m_pLinePoint[0].x()-1,m_pLinePoint[0].y()-1,3,3);
    QRect rectB(m_pLinePoint[1].x()-1,m_pLinePoint[1].y()-1,3,3);
    QRect rectC(m_pLinePoint[2].x()-1,m_pLinePoint[2].y()-1,3,3);
    QRect rectD(m_pLinePoint[3].x()-1,m_pLinePoint[3].y()-1,3,3);

    painter->drawRect(rectA);
    painter->drawRect(rectB);
    painter->drawRect(rectC);
    painter->drawRect(rectD);
    painter->restore();
}

int HConnect::hitSelect(const QPoint &point, bool bSelected)
{
    if(bSelected)
    {
        for(int nSelectPoint = 0; nSelectPoint < 4; nSelectPoint++)
        {
            QPoint iPoint = m_pLinePoint[nSelectPoint];
            QRect rectPoint(iPoint.x()-5,iPoint.y()-5,11,11);
            if(nSelectPoint == 0 || nSelectPoint == 3)
            {
                if(point.x() >= rectPoint.left() && point.x() <= rectPoint.right() && point.y() >= rectPoint.top() && point.y() <= rectPoint.bottom())
                    return nSelectPoint+1;
            }
            rectPoint = QRect(m_pLinePoint[1].x() - 5,m_pLinePoint[1].y(),11,(m_pLinePoint[2].y() - m_pLinePoint[1].y()));
            if(nSelectPoint == 2 || nSelectPoint == 1) //主要是考虑C点可能在B点上方或者在下方
            {
                if(rectPoint.top() > rectPoint.bottom())
                {
                    if(point.x() >= rectPoint.left() && point.x() <= rectPoint.right() && point.y() >= rectPoint.bottom() && point.y() <= rectPoint.top())
                    return nSelectPoint+1;
                }
                else
                {
                    if(point.x() >= rectPoint.left() && point.x() <= rectPoint.right() && point.y() >= rectPoint.top() && point.y() <= rectPoint.bottom())
                        return nSelectPoint+1;
                }
            }
        }
    }
    return 0;
}

void HConnect::moveSelectPoint(int nSelectPoint, HFrame *pFrame, QPoint &point)
{

    switch(nSelectPoint)
    {
    case 1:
        if(point.x() > m_pLinePoint[1].x() )
            return;
        m_pLinePoint[0].setX(point.x());
        m_pLinePoint[0].setY(point.y());
        m_pLinePoint[1].setY(point.y());
        break;
    case 2:
    case 3:
        if(point.x() < m_pLinePoint[0].x() || point.x() > m_pLinePoint[3].x())
            return;
        m_pLinePoint[1].setX(point.x());
        m_pLinePoint[2].setX(point.x());
        break;
    case 4:
        if(point.x() < m_pLinePoint[2].x())
            return;
        m_pLinePoint[3].setX(point.x());
        m_pLinePoint[3].setY(point.y());
        m_pLinePoint[2].setY(point.y());
        break;
    }
    setRectABC();
}

////////////////////////////////////////HInpubObj//////////////////////////////////////
HInputObj::HInputObj(const QRect &rect, HRuleFile *pRuleFile)
                :HDrawObj(rect,pRuleFile)
{
    m_btObjType = TYPE_INPUT;//输入类型
    m_strName = "输入";
    m_pointIn = NULL;
    //m_pointOut = 0;


    m_fOutValue = float(8.0);
    m_bOutValue = 0;

    btInputType = -1;
    m_wMode1 = -1;         //间隔or装置
    m_wStationID1 = -1;    //厂站地址
    m_wProtID1 = -1;       //设备地址
    m_btType1 = -1;
    m_btInType1 = -1;       //遥信 遥测 遥控等
    m_wPointID1 = -1;      //点号
    m_wAttr1 = -1;         //点属性
    fReserved1 = -1;       //保留

    //比较值2
    m_wMode2 = -1;
    m_wStationID2 = -1;
    m_wProtID2 = -1;
    m_btType2 = -1;
    m_btInType2 = -1;//遥信 遥测 遥控等
    m_wPointID2 = -1;
    m_wAttr2 = -1;
    fRating = -1;//测点1的额定值 ???

    //比较值2
    m_fCompValue = 0.0;

    m_btCompType = false;//0:测点比较常数 1:测点比较测点
    m_btCondition = -1;//条件 大于 小于 等于
    calOutPoint();
}

/*
HInputObj::~HInputObj()
{

}*/

void HInputObj::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::readData(nVersion,ds);
    quint8 n8;
    *ds >> n8;
    btInputType = n8;//102:遥信 103:遥测
    quint16 n16;
    *ds >> n16;
    m_wMode1 = n16;         //间隔or装置
    *ds >> n16;
    m_wStationID1 = n16;    //厂站地址
    *ds >> n16;
    m_wProtID1 = n16;       //设备地址
    *ds >> n8;
    m_btType1 = n8;
    *ds >> n8;
    m_btInType1 = n8;       //遥信 遥测 遥控等
    *ds >> n16;
    m_wPointID1 = n16;      //点号
    *ds >> n16;
    m_wAttr1 = n16;         //点属性
    float f;
    *ds >> f;
    fReserved1 = f;       //保留

    //比较值2
    *ds >> n16;
    m_wMode2 = n16;
    *ds >> n16;
    m_wStationID2 = n16;
    *ds >> n16;
    m_wProtID2 = n16;
    *ds >> n8;
    m_btType2 = n8;
    *ds >> n8;
    m_btInType2 = n8;//遥信 遥测 遥控等
    *ds >> n16;
    m_wPointID2 = n16;
    *ds >> n16;
    m_wAttr2 = n16;
    *ds >> f;
    fRating = f;//测点1的额定值 ???

    //比较值2
    *ds >> f;
    m_fCompValue = f;

    *ds >> n8;
    m_btCompType = n8;//0:测点比较常数 1:测点比较测点
    *ds >> n8;
    m_btCondition = n8;//条件 大于 小于 等于
}

void HInputObj::writeData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::writeData(nVersion,ds);
    *ds<<(quint8) btInputType;//102:遥信 103:遥测
    *ds<<(quint16) m_wMode1;         //间隔or装置
    *ds<<(quint16) m_wStationID1;    //厂站地址
    *ds<<(quint16) m_wProtID1;       //设备地址
    *ds<<(quint8) m_btInType1;       //遥信 遥测 遥控等
    *ds<<(quint16) m_wPointID1;      //点号
    *ds<<(quint16) m_wAttr1;         //点属性
    *ds<<(float) fReserved1;       //保留

    //比较值2
    *ds<<(quint16) m_wMode2;
    *ds<<(quint16) m_wStationID2;
    *ds<<(quint16) m_wProtID2;
    *ds<<(quint8) m_btInType2;//遥信 遥测 遥控等
    *ds<<(quint16) m_wPointID2;
    *ds<<(quint16) m_wAttr2;
    *ds<<(float) fRating;//测点1的额定值 ???

    //比较值2
    *ds<<(float) m_fCompValue;

    *ds<<(quint8) m_btCompType;//0:测点比较常数 1:测点比较测点
    *ds<<(quint8) m_btCondition;//条件 大于 小于 等于

}

void HInputObj::draw(QPainter *painter)
{
    HDrawObj::draw(painter);
    calOutPoint();
    QRect rect = m_rectCurPos;

    int nLeftX0 = rect.left();
    int nRightX0 = rect.right();
    int nTopY0 = rect.top();
    int nBottomY0 = rect.bottom();
    
    int nLeftX1 = nLeftX0;
    int nRightX1 = nRightX0 - 25;
    int nTopY1 = nTopY0 + (nBottomY0 - nTopY0)/4;
    int nBottomY1 = nBottomY0 - (nBottomY0 - nTopY0)/4;
    
    painter->save();
    QPen pen;
    pen.setColor(QColor(m_pRuleFile->m_strUpedgeClr));
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    QBrush  brush;
    brush.setColor(QColor(255,255,255));
    brush.setStyle(Qt::SolidPattern);
    if(m_bFill)
    {
        brush.setColor(QColor(m_pRuleFile->m_strFillClr));
    }
    painter->setBrush(brush);
    /*
     pt2* -------* pt1
        |        |   *(pt0,pt5)
     pt3* -------* pt4
    */
    
    QPoint pt[6];
    pt[0].setX(nRightX1);
    pt[0].setY((nTopY1 + nBottomY1)/2);
    pt[1].setX(nRightX1 - 10);
    pt[1].setY(nTopY1);
    pt[2].setX(nLeftX1);
    pt[2].setY(nTopY1);
    pt[3].setX(nLeftX1);
    pt[3].setY(nBottomY1);
    pt[4].setX(nRightX1 - 10);
    pt[4].setY(nBottomY1);
    pt[5] = pt[0];
    painter->drawPolygon(pt,6);
    
    //下边框加粗线
    QPen pen2;
    pen2.setColor(QColor(m_pRuleFile->m_strDownedgeClr));
    pen2.setWidth(2);
    pen2.setStyle(Qt::SolidLine);
    painter->setPen(pen2);
    painter->drawLine(QPoint(nLeftX1+2,nBottomY1),QPoint(nRightX1-10,nBottomY1));
    painter->drawLine(QPoint(nRightX1-10,nBottomY1),QPoint(nRightX1,(nTopY1 + nBottomY1)/2));

    //阴影线
    QPen pen3;
    pen3.setColor(QColor(m_pRuleFile->m_strShadowClr));
    pen3.setWidth(2);
    pen3.setStyle(Qt::SolidLine);
    painter->setPen(pen3);
    painter->drawLine(QPoint(nLeftX1+1,nBottomY1+1),QPoint(nRightX1-10-1,nBottomY1+1));
    painter->drawLine(QPoint(nRightX1-10-1,nBottomY1+1),QPoint(nRightX1-1,(nTopY1+nBottomY1)/2-1));

    //画连接点 此处的矩形没有意义，连接点一般是挨着最右边开始画，除非是逻辑运算 左边也有连接点
    QBrush brush1;
    brush1.setColor(QColor(m_pRuleFile->m_strLineClr));
    brush1.setStyle(Qt::SolidPattern);
    painter->setBrush(brush1);
    QPen pen4;
    pen4.setColor(QColor(m_pRuleFile->m_strLineClr));
    pen4.setWidth(2);
    painter->setPen(pen4);
   // qDebug()<<"nRightX1="<<nRightX1;
    drawPins(painter,QRect(QPoint(nRightX1-60,nTopY0),QPoint(nRightX1,nBottomY0)));//保证处在最右边即可以
    
    //显示ID
    if(m_pRuleFile->bDisplayID)
    {
        QString strID = QString("U%1").arg(m_dwID);
        painter->drawText(QRect(QPoint(nLeftX0,nTopY0),QPoint(nRightX0-10,nTopY1)),Qt::AlignCenter,strID);
    }
    //显示仿真数据
    if(m_pRuleFile->bSimuState)
    {
        painter->save();
        QPen pensim;
        pensim.setColor(QColor(Qt::red));
        pensim.setWidth(1);
        painter->setPen(pensim);
        QString strSim = QString("%1").arg(m_bOutValue);
        painter->drawText(QRect(QPoint(nRightX0-20,nTopY0),QPoint(nRightX0-10,nTopY1)),Qt::AlignCenter,strSim);
        painter->restore();
    }
    //绘制名字
    QPen pen5;
    pen5.setColor(QColor(Qt::black));
    pen5.setWidth(2);
    painter->setPen(pen5);
    if(btInputType == TYPE_INPUT_COMP)
        m_strName = "遥测";
    else if(btInputType == TYPE_INPUT_DIGITAL)
        m_strName = "遥信";
    painter->drawText(QRect(QPoint(nLeftX1,nTopY1),QPoint(nRightX1 - 30,nBottomY1)),Qt::AlignRight | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);
    //绘制连接点名字
    
    painter->restore();
}


void HInputObj::calOutPoint()
{
    QRect rect = m_rectCurPos;
 //   qDebug()<<"out.x = "<<rect.right()<<"out.y"<<rect.top()+rect.height()/2;
    m_pointOut.setX(rect.right());
    m_pointOut.setY(rect.top() + rect.height()/2);
}

void HInputObj::setInputProperty(HFrame *pFrame)
{
    switch(btInputType)
    {
    case TYPE_INPUT_DIGITAL:
        HDigitalProp digitalPorp;
        digitalPorp.exec();
        break;
    }
}

void HInputObj::addInValue(int nNo,bool value)
{

}

bool HInputObj::outValue()//逻辑表达式的值
{
    return m_bOutValue;
}

///////////////////////////////////////////////HResultObj 输出///////////////////////////////
HResultObj::HResultObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 1;
    m_nOutPointSum = 0;
    m_strName = "输出";
    m_pointIn = new QPoint[1];
    calOutPoint();
    m_btObjType = TYPE_RESULT;
}

/*
HResultObj::~HResultObj()
{

}*/

void HResultObj::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::readData(nVersion,ds);
}

void HResultObj::writeData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::writeData(nVersion,ds);
}

void HResultObj::calOutPoint()
{
    QRect rect = m_rectCurPos;
    m_pointOut.setX(rect.right());
    m_pointOut.setY(rect.top() + rect.height()/2);

    m_pointIn[0].setX(rect.left());
    m_pointIn[0].setY(rect.top() + rect.height()/2);
}

void HResultObj::draw(QPainter *painter)
{
    HDrawObj::draw(painter);
    calOutPoint();
    QRect rect = m_rectCurPos;

    int nLeftX0 = rect.left();
    int nRightX0 = rect.right();
    int nTopY0 = rect.top();
    int nBottomY0 = rect.bottom();

    int nLeftX1 = nLeftX0 + 20;
    int nRightX1 = nRightX0;
    int nTopY1 = nTopY0 + (nBottomY0 - nTopY0)/4;
    int nBottomY1 = nBottomY0 - (nBottomY0 - nTopY0)/4;

    painter->save();
    QPen pen;
    pen.setColor(QColor(m_pRuleFile->m_strUpedgeClr));
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    QBrush  brush;
    brush.setColor(QColor(255,255,255));
    brush.setStyle(Qt::SolidPattern);
    if(m_bFill)
    {
        brush.setColor(QColor(m_pRuleFile->m_strFillClr));
    }
    painter->setBrush(brush);
    /*
     pt0,5* -------* pt1
          |        |   *pt2
     pt4  * -------* pt3
    */

    QPoint pt[6];
    pt[0].setX(nLeftX1);
    pt[0].setY(nTopY1);
    pt[1].setX(nRightX1 - 10);
    pt[1].setY(nTopY1);
    pt[2].setX(nRightX1);
    pt[2].setY((nTopY1 + nBottomY1)/2);
    pt[3].setX(nRightX1 - 10);
    pt[3].setY(nBottomY1);
    pt[4].setX(nLeftX1);
    pt[4].setY(nBottomY1);
    pt[5] = pt[0];
    painter->drawPolygon(pt,6);

    //下边框加粗线
    QPen pen2;
    pen2.setColor(QColor(m_pRuleFile->m_strDownedgeClr));
    pen2.setWidth(2);
    pen2.setStyle(Qt::SolidLine);
    painter->setPen(pen2);
    painter->drawLine(QPoint(nLeftX1+2,nBottomY1),QPoint(nRightX1-10,nBottomY1));
    painter->drawLine(QPoint(nRightX1-10,nBottomY1),QPoint(nRightX1,(nTopY1 + nBottomY1)/2));

    //阴影线
    QPen pen3;
    pen3.setColor(QColor(m_pRuleFile->m_strShadowClr));
    pen3.setWidth(2);
    pen3.setStyle(Qt::SolidLine);
    painter->setPen(pen3);
    painter->drawLine(QPoint(nLeftX1+1,nBottomY1+1),QPoint(nRightX1-10-1,nBottomY1+1));
    painter->drawLine(QPoint(nRightX1-10-1,nBottomY1+1),QPoint(nRightX1-1,(nTopY1+nBottomY1)/2-1));

    //画连接点 此处的矩形没有意义，连接点一般是挨着最右边开始画，除非是逻辑运算 左边也有连接点
    QBrush brush1;
    brush1.setColor(QColor(m_pRuleFile->m_strLineClr));
    brush1.setStyle(Qt::SolidPattern);
    painter->setBrush(brush1);
    QPen pen4;
    pen4.setColor(QColor(m_pRuleFile->m_strLineClr));
    painter->setPen(pen4);
   // qDebug()<<"nRightX1="<<nRightX1;
    drawPins(painter,QRect(QPoint(nLeftX1,nTopY0),QPoint(nLeftX1+60,nBottomY0)));//保证处在最右边即可以

    //显示ID
    if(m_pRuleFile->bDisplayID)
    {
        QString strID = QString("U%1").arg(m_dwID);
        painter->drawText(QRect(QPoint(nLeftX0,nTopY0),QPoint(nRightX0-10,nTopY1)),Qt::AlignCenter,strID);
    }
    //显示仿真数据

    //绘制名字
    QPen pen5;
    pen5.setColor(QColor(Qt::black));
    pen5.setWidth(1);
    painter->setPen(pen5);
    painter->drawText(QRect(QPoint(nLeftX1,nTopY1),QPoint(nLeftX1+30,nBottomY1)),Qt::AlignRight | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);
    //绘制连接点名字

    painter->restore();
}

void HResultObj::addInValue(int nNo,bool value)
{
    if(nNo >=0 && nNo < m_nInPointSum)
    {
        m_bInVal[nNo] = value;
    }
}

bool HResultObj::outValue()//逻辑表达式的值
{
    //m_bOutValue
    return false;
}
///////////////////////////////////////////////HOrObj 或///////////////////////////////
HOrObj::HOrObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 2;
    m_nOutPointSum = 1;
    m_strName = "OR";
    m_pointIn = new QPoint[MAXCOUNT_INPUT];
    calOutPoint();
    m_btObjType = TYPE_LOGICOR;
}

/*
HOrObj::~HOrObj()
{

}*/

void HOrObj::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::readData(nVersion,ds);
}

void HOrObj::writeData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::writeData(nVersion,ds);
}

void HOrObj::calOutPoint()
{
    QRect rect ;
    if(m_nInPointSum >=2)
    {
        rect.setLeft(m_rectCurPos.left());
        rect.setRight(m_rectCurPos.right());
        rect.setTop(m_rectCurPos.top() + m_rectCurPos.height()*2/(m_nInPointSum+4));
        rect.setBottom(m_rectCurPos.bottom() - m_rectCurPos.height()/(m_nInPointSum+4));


        m_pointOut.setX(rect.right());
        m_pointOut.setY(rect.top() + rect.height()/2);

        for(int i = 0; i < m_nInPointSum;i++)
        {
            m_pointIn[i].setX(rect.left());
            m_pointIn[i].setY(rect.top() + rect.height()*(i+1)/(m_nInPointSum+1));//上面是小 下面是大}
        }
    }
}


void HOrObj::draw(QPainter *painter)
{
    HDrawObj::draw(painter);
    calOutPoint();
    QRect rect = m_rectCurPos;

    int nLeftX0 = rect.left();
    int nRightX0 = rect.right();
    int nTopY0 = rect.top();
    int nBottomY0 = rect.bottom();

    //主窗口，其实所有的图元连接信息都是在m_rectCurPos这个矩形框绘制完成的
    int nLeftX1 = nLeftX0 + rect.width()/4;
    int nRightX1 = nRightX0 - rect.width()/4;
    int nTopY1 = nTopY0 + rect.height()/(m_nInPointSum + 4);
    int nBottomY1 = nBottomY0 - rect.height()/(m_nInPointSum + 4);;

    //左右两边的矩形
    int nLeftX2 = nLeftX0 + rect.width()/4;
    int nRightX2 = nRightX0 - rect.width()/4;
    int nTopY2 = nTopY0 + rect.height()*2/(m_nInPointSum + 4);//上半部分留着写字
    int nBottomY2 = nBottomY0 - rect.height()/(m_nInPointSum + 4);

    //名字
    int nLeftX3 = nLeftX0 + rect.width()/4;
    int nRightX3 = nRightX0 - rect.width()/4;
    int nTopY3 = nTopY0 + rect.height()/(m_nInPointSum + 4);;
    int nBottomY3 = nTopY0 + rect.height()*2/(m_nInPointSum + 4);;
    //ID 暂时不写
    //value


    //绘制主窗口
    drawMain(painter,QRect(QPoint(nLeftX1,nTopY1),QPoint(nRightX1,nBottomY1)));


    painter->save();
    //绘制针脚
    QBrush brush1;
    brush1.setColor(QColor(m_pRuleFile->m_strLineClr));
    brush1.setStyle(Qt::SolidPattern);
    painter->setBrush(brush1);
    QPen pen1;
    pen1.setColor(QColor(m_pRuleFile->m_strLineClr));
    pen1.setWidth(1);
    pen1.setStyle(Qt::SolidLine);
    painter->setPen(pen1);
    drawPins(painter,QRect(QPoint(nLeftX2,nTopY2),QPoint(nRightX2,nBottomY2)));//保证处在最右边即可以

    if(m_pRuleFile->bDisplayID)
    {
        QString strID = QString("U%1").arg(m_dwID);
        painter->drawText(QRect(QPoint(nLeftX0,nTopY0),QPoint(nRightX0-10,nTopY1)),Qt::AlignCenter,strID);
    }

    QPen pen2(Qt::black);
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawText(QRect(QPoint(nLeftX3,nTopY3),QPoint(nRightX3,nBottomY3)),Qt::AlignCenter | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);

    QFont font("Arial",10);
    painter->setFont(font);
    QString strLabel;
    for(int i = 0; i < m_nInPointSum;i++)
    {
        strLabel = QString("In[%1]").arg(i);
        painter->drawText(QRect(QPoint(nLeftX2,m_pointIn[i].y()-6),QPoint(nLeftX2+20,m_pointIn[i].y()+6)),strLabel);
    }
    painter->drawText(QRect(QPoint(nRightX2-20,m_pointOut.y()-6),QPoint(nRightX2,m_pointOut.y()+6)),"Out");
    painter->restore();
}

void HOrObj::addInValue(int nNo,bool value)
{
    if(nNo >=0 && nNo < m_nInPointSum)
    {
        m_bInVal[nNo] = value;
    }
}

bool HOrObj::outValue()//逻辑表达式的值
{
    m_bOutValue = m_bInVal[0];
    for(int i = 1; i < m_nInPointSum;i++)
        m_bOutValue |= m_bInVal[i];

    return m_bOutValue;
}

///////////////////////////////////////////////HAndObj 与///////////////////////////////
HAndObj::HAndObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 2;
    m_nOutPointSum = 1;
    m_strName = "AND";
    m_pointIn = new QPoint[MAXCOUNT_INPUT];
    calOutPoint();
    m_btObjType = TYPE_LOGICAND;
}

/*
HAndObj::~HAndObj()
{

}*/

void HAndObj::readData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::readData(nVersion,ds);
}

void HAndObj::writeData(int nVersion,QDataStream* ds)
{
    if(!ds) return;
    HDrawObj::writeData(nVersion,ds);
}

void HAndObj::calOutPoint()
{
   QRect rect ;
   if(m_nInPointSum >=2)
   {
       rect.setLeft(m_rectCurPos.left());
       rect.setRight(m_rectCurPos.right());
       rect.setTop(m_rectCurPos.top() + m_rectCurPos.height()*2/(m_nInPointSum+4));
       rect.setBottom(m_rectCurPos.bottom() - m_rectCurPos.height()/(m_nInPointSum+4));


       m_pointOut.setX(rect.right());
       m_pointOut.setY(rect.top() + rect.height()/2);

       for(int i = 0; i < m_nInPointSum;i++)
       {
           m_pointIn[i].setX(rect.left());
           m_pointIn[i].setY(rect.top() + rect.height()*(i+1)/(m_nInPointSum+1));//上面是小 下面是大}
       }
    }
}

void HAndObj::draw(QPainter *painter)
{
    HDrawObj::draw(painter);
    calOutPoint();
    QRect rect = m_rectCurPos;

    int nLeftX0 = rect.left();
    int nRightX0 = rect.right();
    int nTopY0 = rect.top();
    int nBottomY0 = rect.bottom();

    //主窗口，其实所有的图元连接信息都是在m_rectCurPos这个矩形框绘制完成的
    int nLeftX1 = nLeftX0 + rect.width()/4;
    int nRightX1 = nRightX0 - rect.width()/4;
    int nTopY1 = nTopY0 + rect.height()/(m_nInPointSum + 4);
    int nBottomY1 = nBottomY0 - rect.height()/(m_nInPointSum + 4);;

    //左右两边的矩形
    int nLeftX2 = nLeftX0 + rect.width()/4;
    int nRightX2 = nRightX0 - rect.width()/4;
    int nTopY2 = nTopY0 + rect.height()*2/(m_nInPointSum + 4);//上半部分留着写字
    int nBottomY2 = nBottomY0 - rect.height()/(m_nInPointSum + 4);

    //名字
    int nLeftX3 = nLeftX0 + rect.width()/4;
    int nRightX3 = nRightX0 - rect.width()/4;
    int nTopY3 = nTopY0 + rect.height()/(m_nInPointSum + 4);;
    int nBottomY3 = nTopY0 + rect.height()*2/(m_nInPointSum + 4);;
    //ID 暂时不写
    //value

    //绘制主窗口
    drawMain(painter,QRect(QPoint(nLeftX1,nTopY1),QPoint(nRightX1,nBottomY1)));

    painter->save();
    //绘制针脚
    QBrush brush1;
    brush1.setColor(QColor(m_pRuleFile->m_strLineClr));
    brush1.setStyle(Qt::SolidPattern);
    painter->setBrush(brush1);
    QPen pen1;
    pen1.setColor(QColor(m_pRuleFile->m_strLineClr));
    pen1.setWidth(1);
    pen1.setStyle(Qt::SolidLine);
    painter->setPen(pen1);
    drawPins(painter,QRect(QPoint(nLeftX2,nTopY2),QPoint(nRightX2,nBottomY2)));//保证处在最右边即可以

    if(m_pRuleFile->bDisplayID)
    {
        QString strID = QString("U%1").arg(m_dwID);
        painter->drawText(QRect(QPoint(nLeftX0,nTopY0),QPoint(nRightX0-10,nTopY1)),Qt::AlignCenter,strID);
    }

    QFont font1("Arial",10);
    painter->setFont(font1);
    QPen pen2;
    pen2.setColor(QColor(Qt::black));
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawText(QRect(QPoint(nLeftX3,nTopY3),QPoint(nRightX3,nBottomY3)),Qt::AlignCenter | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);

    QFont font("Arial",8);
    painter->setFont(font);
    QString strLabel;
    for(int i = 0; i < m_nInPointSum;i++)
    {
        strLabel = QString("In[%1]").arg(i);
        painter->drawText(QRect(QPoint(nLeftX2,m_pointIn[i].y()-6),QPoint(nLeftX2+20,m_pointIn[i].y()+6)),strLabel);
    }
    painter->drawText(QRect(QPoint(nRightX2-20,m_pointOut.y()-6),QPoint(nRightX2,m_pointOut.y()+6)),"Out");
    painter->restore();
}

void HAndObj::addInValue(int nNo,bool value)
{
    if(nNo >=0 && nNo < m_nInPointSum)
    {
        m_bInVal[nNo] = value;
    }
}

bool HAndObj::outValue()//逻辑表达式的值
{
    m_bOutValue = m_bInVal[0];
    for(int i = 1; i < m_nInPointSum;i++)
        m_bOutValue &= m_bInVal[i];
    return m_bOutValue;
}
