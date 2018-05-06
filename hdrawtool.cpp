#include "hdrawtool.h"
#include "hdrawobj.h"
#include "hframe.h"
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
    //已选中，调整大小
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
    }

    //未选中，选择对象
    if(selectMode == enumNone)
    {
        pObj = pFrame->pRuleFile->objectAt(point);
        if(pObj != NULL)
        {
            selectMode = enumMove;
            if(!pFrame->isSelect(pObj))
            {
                pFrame->select(pObj,b_shiftKey);
            }

        }
    }

    //未选中，选择连接线
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
        selectMode = enumRectSelect;
    }

    m_pointDown = point;
    m_ptDown = point;
    HDrawTool::onMousePress(pFrame,point,e);
}

void HSelectTool::onMouseRelease(HFrame* pFrame,const QPoint& point,QMouseEvent* e)
{
    //2018-4-19
    if(selectMode == enumRectSelect)
    {
        pFrame->selectObjInRect();
        pFrame->m_selectRect = QRect(0,0,0,0);
        pFrame->update();
    }
    else if(selectMode != enumNone)
    {
        //移动且对象唯一
        if(selectMode == enumMove && pFrame->m_selectObjList.count() == 1)
        {
            HDrawObj* pSelObj = (HDrawObj*)pFrame->m_selectObjList.first();
            //判断是否连接对象
            for(int i =0; i < pFrame->pRuleFile->m_drawObjList.count();i++)
            {
                HDrawObj* pObj = pFrame->pRuleFile->m_drawObjList.at(i);
                if(pSelObj == pObj || pObj->m_nOutPointSum == 0) continue;

                //针对or and等有进又出的对象
                for(int k = 0; k < pSelObj->m_nInPointSum;k++)
                {
                    QPoint pt = pSelObj->m_pointIn[k];
                    if(isHaveConnect(pFrame,pObj->m_dwID,pSelObj->m_dwID,k)) continue;
                    QRect rect = QRect(pt,QSize(10,10));
                    if(rect.contains(pObj->m_pointOut))
                    {
                        //增加一条直线
                        addNewConnect(pFrame,pObj,pSelObj,k);
                    }

                }
            }
        }
    }
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
    if(selectMode == enumRectSelect)
        pFrame->m_selectRect = QRect(m_ptDown,point);

    for(int i = 0; i < pFrame->m_selectObjList.count(); i++)
    {
        HDrawObj* pObj = (HDrawObj*)pFrame->m_selectObjList[i];
        QRect rect = pObj->m_rectCurPos;
        if(selectMode == enumMove) //移动的时候绘制
        {
            //两个图元自动连接
            rect.translate(delta);
            pObj->m_rectCurPos = rect.normalized();
            pObj->calOutPoint();
            reCalConnect(pFrame,pObj);
        }
        else if(selectMode == enumSize && g_nSelectPoint != 0) //改变大小的时候绘制
        {
            pObj->moveSelectPoint(g_nSelectPoint,pFrame,localPoint);
            pObj->calOutPoint();
        }
    }
    pFrame->update();
    m_pointDown = localPoint;//左键按得地方发生了改变

}

void HSelectTool::onMouseDoubleClick(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
    if(pFrame->m_selectObjList.count() == 1)
    {
        HDrawObj *pObj = (HDrawObj*)pFrame->m_selectObjList.first();
        /*const QMetaObject *pMObj = pObj->metaObject();
        QString strClassName = pMObj->className();
        if(QString(pMObj->className()).compare("HInputObj") == 0)
        {
            quint8 btInputType = ((HInputObj*)pObj)->btInputType;
            //((HInputObj*)pObj)->setInputProperty(pFrame);
            if(TYPE_INPUT_COMP == btInputType)
            {

            }
        }
        else if(strClassName.compare("HAndObj") == 0 || strClassName.compare("HOrObj") == 0)
        {

        }*/
        pFrame->setDrawObjProp(pObj);

    }
}

void HSelectTool::addNewConnect(HFrame *pFrame, HDrawObj *pInObj, HDrawObj *pOutObj, quint8 btIndex)
{
    if(!pFrame || !pFrame->pRuleFile)
        return;
    QPoint pointOut = pOutObj->m_pointIn[btIndex];//线的out端
    QPoint pointIn = pInObj->m_pointOut;//线的In端
    pOutObj->m_rectCurPos.normalized();
    pOutObj->m_rectCurPos.setLeft(pOutObj->m_rectCurPos.left()+50);
    pOutObj->m_rectCurPos.setRight(pOutObj->m_rectCurPos.right()+50);

    HConnect* newConnect = new HConnect(pInObj->m_dwID,pOutObj->m_dwID,pFrame->pRuleFile,btIndex);
    pFrame->pRuleFile->addConnect(newConnect);

    pOutObj->calOutPoint();
    reCalConnect(pFrame,pOutObj);
}

bool HSelectTool::isHaveConnect(HFrame *pFrame,quint16 wIn, quint16 wOut, quint8 btOutInIndex)
{
    if(!pFrame->pRuleFile)
        return false;
    for(int i = 0; i < pFrame->pRuleFile->m_connectObjList.count();i++)
    {
        HConnect* connect = pFrame->pRuleFile->m_connectObjList.at(i);
        if(connect)
        {
            if(connect->m_dwOutObjID == wOut && connect->m_btOutIndex == btOutInIndex)
                return true;
        }
    }
    return false;
}

void HSelectTool::reCalConnect(HFrame *pFrame, HDrawObj *pObj)
{
    if(!pFrame || !pObj)
         return;
    if(!pFrame->pRuleFile) return;
    for(int i = 0;i < pFrame->pRuleFile->m_connectObjList.count();i++)
    {
        HConnect* conn = pFrame->pRuleFile->m_connectObjList[i];
        if(conn->m_dwInObjID == pObj->m_dwID && pObj->m_nOutPointSum > 0)//遥信遥测等对象
        {
            conn->m_pointIn = pObj->m_pointOut;
            conn->calLine();
        }
        else if(conn->m_dwOutObjID == pObj->m_dwID && pObj->m_nInPointSum > 0) //或 与之类的对象
        {
            conn->m_pointOut = pObj->m_pointIn[conn->m_btOutIndex];
            conn->calLine();
        }
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
         ((HInputObj*)pObj)->setInputType(TYPE_INPUT_COMP);
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
    HDrawTool::drawShape = enumSelection;
    pFrame->pRuleFile->addConnect(pConnectLine);
    pFrame->update();
}

void HConnectTool::onMouseMove(HFrame *pFrame, const QPoint &point, QMouseEvent *e)
{
   pFrame->setCursor(Qt::ArrowCursor);
}



























