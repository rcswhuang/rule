#include "frame.h"
#include "drawobj.h"
#include <QMouseEvent>
#include <QPainter>
#include <QMetaObject>
#include <QDebug>
HFrame::HFrame(QWidget * parent, Qt::WindowFlags f):QFrame(parent,f)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    pRuleFile = new HRuleFile;
}

void HFrame::keyPressEvent(QKeyEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    if(pDrawTool)
        pDrawTool->onKeyPress(event);
}

void HFrame::keyReleaseEvent(QKeyEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    if(pDrawTool)
        pDrawTool->onKeyRelease(event);
}

void HFrame::mousePressEvent(QMouseEvent * event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos();
    if(pDrawTool)
        pDrawTool->onMousePress(this,point,event);
}

void HFrame::mouseReleaseEvent(QMouseEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos();
    if(pDrawTool)
        pDrawTool->onMouseRelease(this,point,event);
}

void HFrame::mouseMoveEvent(QMouseEvent * event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos();
    if(pDrawTool)
        pDrawTool->onMouseMove(this,point,event);
}

void HFrame::mouseDoubleClickEvent(QMouseEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos();
    if(pDrawTool)
        pDrawTool->onMouseDoubleClick(this,point,event);
}

void HFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if(m_bGrid)
        drawGrid(painter);


    for(int i = 0; i < pRuleFile->drawObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)pRuleFile->drawObjList[i];
        pObj->draw(&painter);
        if(isSelect(pObj))//如果某个图元被选择上就画选择框
            pObj->drawSelect(&painter,HDrawObj::selected);
    }

    for(int j = 0; j < pRuleFile->connectObjList.count();j++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->connectObjList[j];
        pConnect->draw(&painter);
        if(isSelect(pConnect))
            pConnect->drawSelect(&painter);
    }
}

void HFrame::setSelectCursor(int nSelectPoint)
{
    switch(nSelectPoint)
    {
    case 1://左上角
    case 5://右下角
        setCursor(Qt::SizeFDiagCursor);
        break;
    case 3://右上角
    case 7://左下角
        setCursor(Qt::SizeBDiagCursor);
        break;
    case 4:
    case 8:
        setCursor(Qt::SizeHorCursor);
        break;
    }
}

void HFrame::setConnectSelectCursor(int nSelectPoint)
{
    switch(nSelectPoint)
    {
    //qDebug()<<"Frame A::nSelectPoint"<<nSelectPoint;
    case 1:
        setCursor(Qt::SizeFDiagCursor);
        break;
    case 4:
        setCursor(Qt::SizeBDiagCursor);
        break;
    case 2:
    case 3:
        setCursor(Qt::SizeHorCursor);
        break;
    }
   // qDebug()<<"Frame B::nSelectPoint"<<nSelectPoint;
}


bool HFrame::select(HDrawObj *pObj,bool bAdd)
{

    if(!bAdd)
    {
        m_selectObjList.clear();
        m_selectConnectObjList.clear();
    }
    if(pObj == NULL || isSelect(pObj))
        return false;

    m_selectObjList.append(pObj);
    update();
}

bool HFrame::selectConnect(HConnect *pObj, bool bAdd)
{
    if(!bAdd)
    {
        m_selectObjList.clear();
        m_selectConnectObjList.clear();
    }
    if(pObj == NULL || isSelect(pObj))
        return false;
    m_selectConnectObjList.append(pObj);
    update();
}

bool HFrame::isSelect(const QObject *pObj)
{
    if(pObj == NULL)
        return false;
    const QMetaObject *pMObj = pObj->metaObject();;
    //qDebug()<<"className:"<<pMObj->className();
    if(QString(pMObj->superClass()->className()).compare("HDrawObj") == 0)
        return m_selectObjList.contains((HDrawObj*)pObj);
    if(QString(pMObj->className()).compare("HConnect") == 0)
       return m_selectConnectObjList.contains((HConnect*)pObj);

    return false;
}

void HFrame::drawGrid(QPainter &painter)
{
    painter.save();
    QPen pen;
    pen.setColor(QColor(0,0,128));
    pen.setWidth(1);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);

    QRect rect1 = rect();
    for(int x = rect1.left() / 100 * 500;x < rect1.right(); x+=20)
    {
        if(x!=0)
            painter.drawLine(QPoint(x,rect1.top()),QPoint(x,rect1.bottom()));
    }

    for(int y = rect1.top() / 100 * 500;y < rect1.bottom(); y+=20)
    {
        if(y!=0)
            painter.drawLine(QPoint(rect1.left(),y),QPoint(rect1.right(),y));
    }
   /* int index = -1;
    for(int x = rect1.left() / 100 * 500;x < rect1.right(); x+=5)
       {
            index++;
           if(x!=0)
               painter.drawLine(QPoint(x+10,rect1.top()),QPoint(x+10,rect1.top() + 5));
           if(index % 5 == 0)
           {
               painter.drawLine(QPoint(x+10,rect1.top()),QPoint(x+10,rect1.top() + 10));
               painter.drawText(QPoint(x+5,rect1.top()+25),QString("%1").arg(index));
           }
       }

    index = -1;
    for(int y = rect1.top() / 100 * 500;y < rect1.bottom(); y+=5)
       {
            index++;
           if(y!=0)
               painter.drawLine(QPoint(rect1.left(),y+10),QPoint(rect1.left() + 5,y+10));

           if(index % 5 == 0)
               painter.drawLine(QPoint(rect1.left(),y+10),QPoint(rect1.left() + 10,y+10));
       }*/
    painter.restore();
}

bool HFrame::isLinkConnectObj(HDrawObj *pObj)
{
    if(pObj == NULL) return false;
    QRect rectDrawObj(QPoint(pObj->m_pointOut.x()-7,pObj->m_pointOut.y() -7),QSize(15,15));
    for(int i = 0; i < pRuleFile->connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->connectObjList[i];
        QRect rectConnectObj(QPoint(pConnect->m_pLinePoint[0].x()-3,pConnect->m_pLinePoint[0].y() -3),QSize(7,7));
        if(rectDrawObj.intersects(rectConnectObj) || pConnect->dwInObjID == pObj->dwID)//在矩形框内(初始)连接或者连接线已经连接之后
        {

            pConnect->m_pointIn.setX(pObj->m_pointOut.x());
            pConnect->m_pLinePoint[0].setX(pObj->m_pointOut.x());
            pConnect->dwInObjID = pObj->dwID;
            if(!isSelect(pConnect))
                pConnect->moveSelectPoint(1,this,pObj->m_pointOut);//移动连接线
        }

        if(pObj->m_nInPointSum > 0)//与 或等结构左边连接
        {
            for(int j =0; j < pObj->m_nInPointSum;j++)
            {
                if(j == pConnect->btOutIndex && pConnect->dwOutObjID == pObj->dwID)
                {
                    pConnect->m_pointOut.setX(pObj->m_pointIn[pConnect->btOutIndex].x());
                    pConnect->m_pLinePoint[3].setX(pObj->m_pointIn[pConnect->btOutIndex].x());
                    pConnect->dwOutObjID = pObj->dwID;
                    if(!isSelect(pConnect))
                         pConnect->moveSelectPoint(4,this,pObj->m_pointIn[j]);
                }
                QRect rect(QPoint(pObj->m_pointIn[j].x()-3,pObj->m_pointIn[j].y()-3),QSize(7,7));
                QRect rectConnect(QPoint(pConnect->m_pLinePoint[3].x()-1,pConnect->m_pLinePoint[3].y() -1),QSize(3,3));
                //必须要增加判断当前针脚点是否已经连接
              if(rect.intersects(rectConnect) )
                {
                    if(findConnectInRect(pObj,j,0)/* && pConnect->dwOutObjID == pObj->dwID*/)
                    {
                        continue;
                    }
                    pConnect->m_pointOut.setX(pObj->m_pointIn[pConnect->btOutIndex].x());
                    pConnect->m_pLinePoint[3].setX(pObj->m_pointIn[j].x());
                    pConnect->dwOutObjID = pObj->dwID;
                    pConnect->btOutIndex = j;
                    break;
                }
            }
        }
    }
    return true;
}


bool HFrame::findConnectInRect(const HDrawObj*pObj,const quint16 wIndex,int nMoveDirect)
{
    bool bfind = false;
    for(int i = 0; i < pRuleFile->connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->connectObjList[i];
        if(pConnect->btOutIndex == wIndex && pConnect->dwOutObjID == pObj->dwID)//在矩形框内(初始)连接或者连接线已经连接之后
        {
            bfind = true;
            break;
        }
    }
    return bfind;

}


bool HFrame::objOutConnectLine(HDrawObj* pObj,const int &deltaX,const QPoint &localpoint)
{
    bool bfind = false;
    for(int i = 0; i < pRuleFile->connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->connectObjList[i];
        if(pConnect->dwInObjID == pObj->dwID)//在矩形框内(初始)连接或者连接线已经连接之后
        {
            //先要求出当前按下去的点 到 out点距离
           //int deltaX = pObj->m_rectCurPos.width();// - localpoint.x();
           //qDebug() <<"pObj->m_pointOut.x()"<<pObj->m_pointOut.x()<< "x" << localpoint.x() << "m[1]" << pConnect->m_pLinePoint[1].x() ;
           if(localpoint.x() + deltaX > pConnect->m_pLinePoint[1].x() && !isSelect(pConnect))
           {
               bfind = true;
               break;
           }
        }
        if(pConnect->dwOutObjID == pObj->dwID)
        {
            if(pObj->m_nInPointSum > 0)
            {
                if(localpoint.x() - deltaX < pConnect->m_pLinePoint[2].x() && !isSelect(pConnect))
                {
                    bfind = true;
                    break;
                }
            }
        }
    }
    return bfind;

}











