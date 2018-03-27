#include "drawtool.h"
#include "drawobj.h"
#include "frame.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QDebug>
static QList<HDrawTool*> m_drawTooList;
static HRectTool InputDigTool(enumInputDIGITAL);
static HRectTool InputAnaTool(enumInputANALOGUE);
static HRectTool ResultTool(enumOutRESUTL);
static HRectTool ORTool(enumLogicOR);
static HRectTool AndTool(enumLogicAND);
static HSelectTool selectTool;
static HConnectTool connectTool;

DRAWSHAPE HDrawTool::drawShape = enumSelection;



HDrawTool::HDrawTool(DRAWSHAPE drawShape)
{
    b_shiftKey = false;
    b_mouseLeftKey = false;
    this->m_nDrawShape = drawShape;
    m_drawTooList.append(this);
}


HDrawTool* HDrawTool::findTool(DRAWSHAPE drawshape)
{
    for(int i = 0; i < m_drawTooList.count();i++)
    {
        HDrawTool *pDrawTool = (HDrawTool*)m_drawTooList[i];
        if(pDrawTool->m_nDrawShape == drawshape)
            return pDrawTool;
    }

    return NULL;
}

void HDrawTool::onMousePress(HFrame *pView, const QPoint &point, QMouseEvent* e)
{
    //记得转化坐标
   // m_pointDown = point;
    //m_pointUp = point;
    //point = pScene->map
    b_mouseLeftKey = true;

}


void HDrawTool::onMouseRelease(HFrame *pView, const QPoint &point, QMouseEvent* e)
{
    b_mouseLeftKey = false;
    if(point == m_pointDown)
        drawShape = enumSelection;
}


void HDrawTool::onMouseMove(HFrame *pFrame, const QPoint &point, QMouseEvent* e)
{
    //m_pointMove = point;
    //设置默认状态的鼠标
    pFrame->setCursor(Qt::ArrowCursor);

}

void HDrawTool::onKeyPress(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
        b_shiftKey = true;
}

void HDrawTool::onKeyRelease(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Shift)
        b_shiftKey = false;
}
///////////////////////////////////////////////HSelectTool//////////////////////////////////////
SELECTMODE selectMode = enumNone;
int g_nSelectPoint = 0;

HSelectTool::HSelectTool():HDrawTool(enumSelection)
{

}


void HSelectTool::onMousePress(HFrame* pFrame,const QPoint& point,QMouseEvent* e)
{
    HDrawObj* pObj = NULL;
    HConnect* pConnectObj = NULL;
    selectMode = enumNone;
    //处于调整大小位置
    if(pFrame->m_selectObjList.count() == 1)
    {
        HDrawObj* pObj = (HDrawObj*)pFrame->m_selectObjList.first();
        g_nSelectPoint = pObj->hitSelect(point,true);
        if(g_nSelectPoint != 0)
            selectMode = enumSize;
    }

    if(pFrame->m_selectConnectObjList.count() == 1)
    {
        HConnect* pObj = (HConnect*)pFrame->m_selectConnectObjList.first();
        g_nSelectPoint = pObj->hitSelect(point,true);
        if(g_nSelectPoint != 0)
            selectMode = enumSize;
        //else if(g_nSelectPoint == 2 || g_nSelectPoint == 3)
        //    selectMode = enumMove;
    }
    //选择对象，处于选中状态，按住shift键将多选
    if(selectMode == enumNone)
    {
        pObj = pFrame->pRuleFile->objectAt(point);
        if(pObj != NULL)
        {
            selectMode = enumMove;
            pObj->nRightDeltaX = pObj->m_pointOut.x() - point.x();
            if(pObj->m_nInPointSum > 0)
                pObj->nLeftDeltaX = point.x() - pObj->m_pointIn[0].x();
            if(!pFrame->isSelect(pObj))
            {
                pFrame->select(pObj,b_shiftKey);
            }

        }
    }

    //connect
    if(selectMode == enumNone)
    {
        pConnectObj = pFrame->pRuleFile->connectAt(point);
        if(pConnectObj != NULL)
        {
            selectMode = enumMove;
            if(!pFrame->isSelect(pConnectObj))
            {
                pFrame->selectConnect(pConnectObj,b_shiftKey);
            }
        }

    }

    //未选择对象，则删除选中对象列表 //connect
    if(selectMode == enumNone)
    {
            pFrame->selectConnect(NULL);
    }


    //未选择对象，则删除选中对象列表 //HDrawObj
    if(selectMode == enumNone)
    {
        if(!b_shiftKey)
            pFrame->select(NULL);
    }

    m_pointDown = point;
    HDrawTool::onMousePress(pFrame,point,e);
}

void HSelectTool::onMouseRelease(HFrame* pFrame,const QPoint& point,QMouseEvent* e)
{
    pFrame->update();
    HDrawTool::onMouseRelease(pFrame,point,e);
}


void HSelectTool::onMouseMove(HFrame* pFrame,const QPoint& point,QMouseEvent* e)
{

    //主要要判左键没有按下去的情况 主要是移动到选择的小区域的时候，改变鼠标的状态
    if(!b_mouseLeftKey)
    {
        if(drawShape == enumSelection && pFrame->m_selectObjList.count() > 0)
        {

            HDrawObj* pObj = pFrame->m_selectObjList.first();
            int selectedPoint = pObj->hitSelect(point,true);
            if(selectedPoint != 0)
            {
                pFrame->setSelectCursor(selectedPoint);
                return;
            }
        }
        if(drawShape == enumSelection && pFrame->m_selectConnectObjList.count() > 0)
        {
            HConnect* pConObj = pFrame->m_selectConnectObjList.first();
            int selectedConnect = pConObj->hitSelect(point,true);
            //qDebug()<<"selectConnect:selectedConnect = "<<selectedConnect;
            if(selectedConnect != 0)
            {
                pFrame->setConnectSelectCursor(selectedConnect);
                return;
            }
            //return表示返回 整个部分是表示左键没有按下时候的移动
        }
        if(drawShape == enumSelection)
         {
            //HConnectTool::onMouseMove(pFrame,point,e);
            HDrawTool::onMouseMove(pFrame,point,e);
        }
        return;
    }
    //以下是左键按下去的时候
    QPoint localPoint = point;
    QPoint delta = (QPoint)(localPoint - m_pointDown);
    for(int i = 0; i < pFrame->m_selectObjList.count(); i++)
    {
        HDrawObj* pObj = (HDrawObj*)pFrame->m_selectObjList[i];
        QRect rect = pObj->m_rectCurPos;
        if(selectMode == enumMove) //移动的时候绘制
        {
            //判断 如果全部都选了的话 就要都移动 而不是只移动一部分
            //还要判一下 如果当前状态是连接的，HDrawObj对象超过连接线的部分时要停止移动 防止跑到连接线的右边
            if((pObj->m_nInPointSum > 0 && !pFrame->objOutConnectLine(pObj,pObj->nLeftDeltaX,localPoint)) || !pFrame->objOutConnectLine(pObj,pObj->nRightDeltaX,localPoint) )
            {
                qDebug()<<"pObj->m_nInPointSum"<<pObj->m_nInPointSum;
                rect.moveTo(rect.x() + delta.x(),rect.y() + delta.y());
                pObj->m_rectCurPos = rect;
                pObj->calOutPoint();
                pFrame->isLinkConnectObj(pObj);
           }
            //判断当前Obj是否已经连接对象，如果已经连接，移动时，连接的直线发生变化

        }
        else if(selectMode == enumSize && g_nSelectPoint != 0) //改变大小的时候绘制
        {
            pObj->moveSelectPoint(g_nSelectPoint,pFrame,localPoint);
            pObj->calOutPoint();
        }
        pFrame->update();
    }

    for(int i = 0; i < pFrame->m_selectConnectObjList.count(); i++)
    {
        HConnect* pConnect = (HConnect*)pFrame->m_selectConnectObjList[i];
        //qDebug()<<"selectConnect:selectMode = "<<selectMode;
        if(selectMode == enumMove) //移动的时候绘制
        {
            if((pConnect->dwOutObjID == (quint16)-1 && pConnect->dwInObjID == (quint16)-1))
            {
                pConnect->moveTo(delta.x(),delta.y());
            }
            HDrawObj* pObj = pFrame->pRuleFile->findDrawObj(pConnect->dwInObjID);
            HDrawObj* pObj1 = pFrame->pRuleFile->findDrawObj(pConnect->dwOutObjID);
            if(pObj)
            {
                //如果connect线左边对象选择同时右边有对象的时候但没有选择
                if(pConnect->dwInObjID == pObj->dwID && pFrame->isSelect(pObj) && pObj1 != NULL && !pFrame->isSelect(pObj1) && pConnect->dwOutObjID == pObj1->dwID)
                {
                    //pConnect->moveTo(delta.x(),delta.y());
                    pConnect->moveSelectPoint(1,pFrame,pObj->m_pointOut);
                }
                else if(pConnect->dwInObjID == pObj->dwID && pFrame->isSelect(pObj) && pObj1 == NULL)
                {
                     pConnect->moveTo(delta.x(),delta.y());
                }
            }

            if(pObj1)
            {
                if(pConnect->dwOutObjID == pObj1->dwID && pFrame->isSelect(pObj1) &&pObj != NULL && !pFrame->isSelect(pObj) && pConnect->dwInObjID == pObj->dwID)
                {
                    pConnect->moveSelectPoint(4,pFrame,pObj1->m_pointIn[pConnect->btOutIndex]);
                }
                else if(pConnect->dwOutObjID == pObj1->dwID && pFrame->isSelect(pObj1) && pObj == NULL)
                {
                     pConnect->moveTo(delta.x(),delta.y());
                }
            }

            if(pObj && pObj1)
            {
                if(pConnect->dwInObjID == pObj->dwID && pFrame->isSelect(pObj)  && pConnect->dwOutObjID == pObj1->dwID && pFrame->isSelect(pObj1) )
                {
                    pConnect->moveTo(delta.x(),delta.y());
                }
            }


            //判断是否移动进当前连接对象的范围，如果进入则自动连接
            //判断线是否移动出当前连接对象的范围，如果移除则删除连接点

        }
        else if(selectMode == enumSize && g_nSelectPoint != 0) //改变大小的时候绘制
        {
            pConnect->moveSelectPoint(g_nSelectPoint,pFrame,localPoint);
        }
        pFrame->update();
    }

    m_pointDown = localPoint;//左键按得地方发生了改变

}

void HSelectTool::onMouseDoubleClick(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
    if(pFrame->m_selectObjList.count() == 1)
    {
        HDrawObj *pObj = (HDrawObj*)pFrame->m_selectObjList.first();
        const QMetaObject *pMObj = pObj->metaObject();
        if(QString(pMObj->className()).compare("HInputObj") == 0)
            ((HInputObj*)pObj)->setInputProperty(pFrame);
    }
}

///////////////////////////////////////////////HRectTool///////////////////////////////////////
HRectTool::HRectTool(DRAWSHAPE drawshape):HDrawTool(drawshape)
{

}

void HRectTool::onMousePress(HFrame *pView, const QPoint &point, QMouseEvent* e)
{
    HDrawTool::onMousePress(pView,point,e);
}

void HRectTool::onMouseRelease(HFrame *pFrame, const QPoint &point, QMouseEvent* e)
{
     HRuleFile* pRuleFile = pFrame->pRuleFile;
     HDrawTool::onMouseRelease(pFrame,point,e);
     HDrawObj *pObj = NULL;
     switch(m_nDrawShape)
     {
     case enumInputDIGITAL:
         pObj = new HInputObj(QRect(point,QSize(100,40)),pRuleFile);
         ((HInputObj*)pObj)->setInputType(TYPE_INPUT_DIGITAL);
         break;
     case enumInputANALOGUE:
         pObj = new HInputObj(QRect(point,QSize(100,40)),pRuleFile);
         ((HInputObj*)pObj)->setInputType(TYPE_INPUT_ANALOGUE);
         break;
     case enumOutRESUTL:
         if(pFrame->pRuleFile->resultObj() == NULL)
             pObj = new HResultObj(QRect(point,QSize(100,40)),pRuleFile);
         else
             pObj = NULL;
         break;
     case enumLogicOR:
         pObj = new HOrObj(QRect(point,QSize(80,84)),pRuleFile);
         break;
     case enumLogicAND:
         pObj = new HAndObj(QRect(point,QSize(80,84)),pRuleFile);
         break;
     }

     if(pObj != NULL)
     {
         pRuleFile->add(pObj);
         pFrame->select(pObj);
     }
     HDrawTool::drawShape = enumSelection;
     selectTool.onMouseRelease(pFrame,point,e);
     //pView->update();
}

void HRectTool::onMouseMove(HFrame *pView, const QPoint &point, QMouseEvent* e)
{

}


//////////////////////////////////////////////////HConnectTool////////////////////////////////////////
HConnectTool::HConnectTool():HDrawTool(enumConnect)
{
}

//q
void HConnectTool::onMousePress(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
    HConnect* pObj = NULL;
    selectMode = enumNone;
    //处于调整大小位置
  /*  if(pFrame->m_selectConnectObjList.count() == 1)
    {
        HConnect* pObj = (HConnect*)pFrame->m_selectConnectObjList.first();
        g_nSelectPoint = pObj->hitSelect(point,true);
        if(g_nSelectPoint != 0)
            selectMode = enumSize;
    }*/
    //选择对象，处于选中状态，按住shift键将多选
    if(selectMode == enumNone)
    {
        //qDebug()<<"select obj1";
        pObj = pFrame->pRuleFile->connectAt(point);
        if(pObj != NULL)
        {
            selectMode = enumMove;
            if(!pFrame->isSelect(pObj))
            {
                pFrame->selectConnect(pObj);
                //qDebug()<<"select obj";
            }
        }

    }

    //未选择对象，则删除选中对象列表
    if(selectMode == enumNone)
    {
            pFrame->select(NULL);
    }

    m_pointDown = point;
    HDrawTool::onMousePress(pFrame,point,e);
}

void HConnectTool::onMouseRelease(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
    HDrawTool::onMouseRelease(pFrame,point,e);
    HConnect *pConnectLine = NULL;
    pConnectLine = new HConnect(pFrame->pRuleFile);
    pConnectLine->calLine(point);
    //pConnectLine->draw();
    HDrawTool::drawShape = enumSelection;
    pFrame->pRuleFile->addConnect(pConnectLine);
    pFrame->update();
}

void HConnectTool::onMouseMove(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
   pFrame->setCursor(Qt::ArrowCursor);
}



























