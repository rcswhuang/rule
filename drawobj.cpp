#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "drawobj.h"
#include <QPen>
#include <QPoint>
#include <QBrush>
//#include "frame.h"
#include "inputprop.h"
#include <math.h>
#include <QDebug>

HDrawObj::HDrawObj(QObject *parent) : QObject(parent)
{

}

HDrawObj::HDrawObj(const QRect &rect, HRuleFile *pRuleFile)
{
    m_bConnect = false;
    m_btObjType = TYPE_NONE; //图符类型
    m_strName=""; //图元名称
    m_strRuleName = ""; //规则文件名称
    m_bOutValue = false;//逻辑输出
    m_fOutValue = 0.0;//模拟量输出

    m_rectCurPos = rect; //当前点矩形位置
    m_pRuleFile = pRuleFile; //规则文件
    m_pointIn = NULL; //多点输入
    //m_pointOut; //实际上对外的连接点位置 对外只有一个输出点
    m_nInPointSum = 0; //输入点个数
    m_nOutPointSum = 1; //输出点个数
    m_bFill = true; //是否填充
    m_clrFill = QColor(206,230,255); //填充色
    m_clrLine = QColor(0,128,128); //线条颜色
    m_clrText = QColor(0,0,255); //文字颜色
    m_clrUpedge = QColor(255,255,255); //上边框颜色
    m_clrDownedge = QColor(0,0,0); //下边框颜色
    m_clrShadow = QColor(128,128,128); //阴影颜色
    if(pRuleFile)
    {

        dwID = pRuleFile->generateDrawObjID();
        qDebug()<<"dwID = "<<dwID;
    }
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
    QBrush brush(m_clrFill);
    QPen pen(m_clrUpedge);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    //pen.setBrush(brush);

    painter->setPen(pen);
    painter->setBrush(brush);
    painter->drawRect(rectMain);

    //边缘
    QPen pen2(m_clrDownedge);
    pen2.setStyle(Qt::SolidLine);
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawLine(QPoint(rectMain.left()+2,rectMain.bottom()),QPoint(rectMain.right(),rectMain.bottom()));
    painter->drawLine(QPoint(rectMain.right(),rectMain.bottom()),QPoint(rectMain.right(),rectMain.top()-2));

    //阴影
    QPen pen3(m_clrShadow);
    pen3.setStyle(Qt::SolidLine);
    pen3.setWidth(2);
    painter->setPen(pen3);
    painter->drawLine(QPoint(rectMain.left()+1,rectMain.bottom()+1),QPoint(rectMain.right()-1,rectMain.bottom()+1));
    painter->drawLine(QPoint(rectMain.right()-1,rectMain.bottom()+1),QPoint(rectMain.right()-1,rectMain.top()-1));

    //线条
    QPen pen4(m_clrDownedge);
    pen4.setStyle(Qt::SolidLine);
    pen4.setWidth(1);
    painter->setPen(pen4);
    painter->drawLine(QPoint(rectMain.left()+2,rectMain.top() + rectMain.height()/(m_nInPointSum+2)),QPoint(rectMain.right()-2,rectMain.top() + rectMain.height()/(m_nInPointSum + 2)));
    QPen pen5(m_clrShadow);
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
    case TYPE_INPUT:
        strOperator = m_strName;
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
    case TYPE_INPUT:
        strOperator = " ";
        break;
    }
    return strOperator;
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
    HDrawObj* pObjIn;
    HDrawObj* pObjOut;
    m_pointIn = pObjIn->m_pointOut;
    m_pointOut = pObjOut->m_pointIn[btInIndex];
    btOutIndex = btInIndex;
    m_btSelLine = 0;

    //calLine();
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
void HConnect::calLine(const QPoint &point,int deltaX,int deltaY,bool bStart)
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
        m_pLinePoint[0] = QPoint(point.x()-1,point.y());//A点
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
        m_pLinePoint[3] = QPoint(pointOut.x()-10,pointIn.y()+5*10);
        m_pLinePoint[4] = QPoint(pointOut.x()-10,pointOut.y());
        m_pLinePoint[5] = pointOut;
        m_btPointSum = 6;
    }
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
    if(!m_pLinePoint)
        pen.setColor(QColor(20,180,50));
    else
        pen.setColor(QColor(255,0,255));
    painter->setPen(pen);
    painter->drawPolygon(m_pLinePoint,m_btPointSum);
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
    wMode1 = -1;         //间隔or装置
    wStationID1 = -1;    //厂站地址
    wProtID1 = -1;       //设备地址
    btInType1 = -1;       //遥信 遥测 遥控等
    wPointID1 = -1;      //点号
    wAttr1 = -1;         //点属性
    fReserved1 = -1;       //保留

    //比较值2
    wMode2 = -1;
    wStationID2 = -1;
    wProtID2 = -1;
    btInType2 = -1;//遥信 遥测 遥控等
    wPointID2 = -1;
    wAttr2 = -1;
    fRating = -1;//测点1的额定值 ???

    //比较值2
    m_fCompValue = 0.0;

    bCompType = false;//0:测点比较常数 1:测点比较测点
    btCondition = -1;//条件 大于 小于 等于
    calOutPoint();
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
    QPen pen(m_clrUpedge);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    QBrush  brush(QColor(255,255,255));
    if(m_bFill)
    {
        brush.setColor(m_clrFill);
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
    QPen pen2(m_clrDownedge);
    pen2.setWidth(2);
    pen2.setStyle(Qt::SolidLine);
    painter->setPen(pen2);
    painter->drawLine(QPoint(nLeftX1+2,nBottomY1),QPoint(nRightX1-10,nBottomY1));
    painter->drawLine(QPoint(nRightX1-10,nBottomY1),QPoint(nRightX1,(nTopY1 + nBottomY1)/2));

    //阴影线
    QPen pen3(m_clrShadow);
    pen3.setWidth(2);
    pen3.setStyle(Qt::SolidLine);
    painter->setPen(pen3);
    painter->drawLine(QPoint(nLeftX1+1,nBottomY1+1),QPoint(nRightX1-10-1,nBottomY1+1));
    painter->drawLine(QPoint(nRightX1-10-1,nBottomY1+1),QPoint(nRightX1-1,(nTopY1+nBottomY1)/2-1));

    //画连接点 此处的矩形没有意义，连接点一般是挨着最右边开始画，除非是逻辑运算 左边也有连接点
    QBrush brush1(m_clrLine);
    painter->setBrush(brush1);
    QPen pen4(m_clrLine);
    pen4.setWidth(2);
    painter->setPen(pen4);
   // qDebug()<<"nRightX1="<<nRightX1;
    drawPins(painter,QRect(QPoint(nRightX1-60,nTopY0),QPoint(nRightX1,nBottomY0)));//保证处在最右边即可以
    
    //显示ID
    
    //显示仿真数据
    
    //绘制名字
    QPen pen5(m_clrText);
    pen5.setWidth(2);
    painter->setPen(pen5);
    if(btInputType == TYPE_INPUT_ANALOGUE)
        m_strName = "遥测输入";
    else if(btInputType == TYPE_INPUT_DIGITAL)
        m_strName = "遥信输入";
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
        HInputProp digitalPorp;
        digitalPorp.exec();
        break;
    }
}



///////////////////////////////////////////////HResultObj 输出///////////////////////////////
HResultObj::HResultObj(QObject *parent)
{

}

HResultObj::HResultObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 1;
    m_nOutPointSum = 0;
    m_strName = "输出";
    m_pointIn = new QPoint[1];
    calOutPoint();
    m_btObjType = TYPE_RESULT;
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
    QPen pen(m_clrUpedge);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);

    QBrush  brush(QColor(255,255,255));
    if(m_bFill)
    {
        brush.setColor(m_clrFill);
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
    QPen pen2(m_clrDownedge);
    pen2.setWidth(2);
    pen2.setStyle(Qt::SolidLine);
    painter->setPen(pen2);
    painter->drawLine(QPoint(nLeftX1+2,nBottomY1),QPoint(nRightX1-10,nBottomY1));
    painter->drawLine(QPoint(nRightX1-10,nBottomY1),QPoint(nRightX1,(nTopY1 + nBottomY1)/2));

    //阴影线
    QPen pen3(m_clrShadow);
    pen3.setWidth(2);
    pen3.setStyle(Qt::SolidLine);
    painter->setPen(pen3);
    painter->drawLine(QPoint(nLeftX1+1,nBottomY1+1),QPoint(nRightX1-10-1,nBottomY1+1));
    painter->drawLine(QPoint(nRightX1-10-1,nBottomY1+1),QPoint(nRightX1-1,(nTopY1+nBottomY1)/2-1));

    //画连接点 此处的矩形没有意义，连接点一般是挨着最右边开始画，除非是逻辑运算 左边也有连接点
    QBrush brush1(m_clrLine);
    painter->setBrush(brush1);
    QPen pen4(m_clrLine);
    painter->setPen(pen4);
   // qDebug()<<"nRightX1="<<nRightX1;
    drawPins(painter,QRect(QPoint(nLeftX1,nTopY0),QPoint(nLeftX1+60,nBottomY0)));//保证处在最右边即可以

    //显示ID

    //显示仿真数据

    //绘制名字
    QPen pen5(m_clrText);
    pen5.setWidth(1);
    painter->setPen(pen5);
    painter->drawText(QRect(QPoint(nLeftX1,nTopY1),QPoint(nLeftX1+30,nBottomY1)),Qt::AlignRight | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);
    //绘制连接点名字

    painter->restore();
}


///////////////////////////////////////////////HOrObj 或///////////////////////////////
HOrObj::HOrObj(QObject *parent)
{

}

HOrObj::HOrObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 2;
    m_nOutPointSum = 1;
    m_strName = "或";
    m_pointIn = new QPoint[2];
    calOutPoint();
    m_btObjType = TYPE_LOGICOR;
}

void HOrObj::calOutPoint()
{
    QRect rect = m_rectCurPos;
    m_pointOut.setX(rect.right());
    m_pointOut.setY(rect.top() + rect.height()/2);

   reCalOrInputPoint();
}

void HOrObj::reCalOrInputPoint()
{
    if(m_pointIn != NULL)
    {
        delete[] m_pointIn;
        m_pointIn = NULL;
    }
    QRect rect ;
    rect.setLeft(m_rectCurPos.left());
    rect.setRight(m_rectCurPos.right());
    rect.setTop(m_rectCurPos.top() + m_rectCurPos.height()/(m_nInPointSum+4));
    rect.setBottom(m_rectCurPos.bottom() - m_rectCurPos.height()*2/(m_nInPointSum+4));


    m_pointIn = new QPoint[m_nOutPointSum];
    int nFenMu = m_nOutPointSum + 1;//分母
    for(int i = 0; i < m_nInPointSum;i++)
    {
        m_pointIn[i].setX(rect.left());
        m_pointIn[i].setY(rect.top() + rect.height()*(m_nInPointSum-i)/nFenMu);//上面是小 下面是大
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
    QBrush brush1(m_clrLine);
    painter->setBrush(brush1);
    QPen pen1(m_clrLine);
    pen1.setWidth(1);
    pen1.setStyle(Qt::SolidLine);
    painter->setPen(pen1);
    drawPins(painter,QRect(QPoint(nLeftX2,nTopY2),QPoint(nRightX2,nBottomY2)));//保证处在最右边即可以

    QPen pen2(m_clrText);
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawText(QRect(QPoint(nLeftX3,nTopY3),QPoint(nRightX3,nBottomY3)),Qt::AlignCenter | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);



    painter->restore();
}

///////////////////////////////////////////////HAndObj 与///////////////////////////////
HAndObj::HAndObj(QObject *parent)
{

}

HAndObj::HAndObj(const QRect &rect, HRuleFile *pRuleFile):HDrawObj(rect,pRuleFile)
{
    m_nInPointSum = 2;
    m_nOutPointSum = 1;
    m_strName = "与";
    m_pointIn = new QPoint[MAXCOUNT_INPUT];
    calOutPoint();
    m_btObjType = TYPE_LOGICAND;
}

void HAndObj::calOutPoint()
{
   QRect rect ;
   if(m_nOutPointSum >=2)
   {
       rect.setLeft(m_rectCurPos.left());
       rect.setRight(m_rectCurPos.right());
       rect.setTop(m_rectCurPos.top() + m_rectCurPos.height()/(m_nInPointSum+4));
       rect.setBottom(m_rectCurPos.bottom() - m_rectCurPos.height()*2/(m_nInPointSum+4));


       m_pointOut.setX(rect.right());
       m_pointOut.setY(rect.top() + rect.height()/2);

       for(int i = 0; i < m_nInPointSum;i++)
       {
           m_pointIn[i].setX(rect.left());
           m_pointIn[i].setY(rect.top() + rect.height()*(m_nInPointSum-i)/(m_nInPointSum+1));//上面是小 下面是大}
       }
    }
}

#include <QDebug>
void HAndObj::reCalOrInputPoint()
{

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
    QBrush brush1(m_clrLine);
    painter->setBrush(brush1);
    QPen pen1(m_clrLine);
    pen1.setWidth(1);
    pen1.setStyle(Qt::SolidLine);
    painter->setPen(pen1);
    drawPins(painter,QRect(QPoint(nLeftX2,nTopY2),QPoint(nRightX2,nBottomY2)));//保证处在最右边即可以

    QPen pen2(m_clrText);
    pen2.setWidth(2);
    painter->setPen(pen2);
    painter->drawText(QRect(QPoint(nLeftX3,nTopY3),QPoint(nRightX3,nBottomY3)),Qt::AlignCenter | Qt::AlignVCenter| Qt::TextSingleLine,m_strName);



    painter->restore();
}

