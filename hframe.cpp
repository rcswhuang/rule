#include "hframe.h"
#include "hdrawobj.h"
#include <QMouseEvent>
#include <QPainter>
#include <QMetaObject>
#include <QScrollBar>
#include <QProcessEnvironment>
#include <QDir>
#include "hlogicprop.h"
#include "hcompareprop.h"
#include "hdigitalprop.h"
#include <QDebug>

HFrame::HFrame(QScrollArea* scrollArea,QWidget * parent, Qt::WindowFlags f):
    m_pScrollArea(scrollArea),QFrame(parent,f)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    factor = 1.0;
    pRuleFile = NULL;
    m_selectRect = QRect(0,0,0,0);
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
    QPoint point = event->pos()/factor;
    if(pDrawTool)
        pDrawTool->onMousePress(this,point,event);
}

void HFrame::mouseReleaseEvent(QMouseEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos()/factor;
    if(pDrawTool)
        pDrawTool->onMouseRelease(this,point,event);
}

void HFrame::mouseMoveEvent(QMouseEvent * event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos()/factor;
    if(pDrawTool)
        pDrawTool->onMouseMove(this,point,event);
}

void HFrame::mouseDoubleClickEvent(QMouseEvent *event)
{
    HDrawTool *pDrawTool = HDrawTool::findTool(HDrawTool::drawShape);
    QPoint point = event->pos()/factor;
    if(pDrawTool)
        pDrawTool->onMouseDoubleClick(this,point,event);
}

void HFrame::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();
    painter.scale(factor,factor);
    QPalette pale = palette();
    pale.setColor(QPalette::Background,QColor(Qt::white));
    setPalette(pale);

    if(pRuleFile->m_bGrid)
        drawGrid(painter);


    for(int i = 0; i < pRuleFile->m_drawObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)pRuleFile->m_drawObjList[i];
        pObj->painter(&painter);
        if(isSelect(pObj))//如果某个图元被选择上就画选择框
            pObj->drawSelect(&painter,HDrawObj::selected);
    }

    for(int j = 0; j < pRuleFile->m_connectObjList.count();j++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->m_connectObjList[j];
        pConnect->painter(&painter);
        if(isSelect(pConnect))
            pConnect->drawSelect(&painter);
    }
    painter.restore();

    if(m_selectRect.width() > 0 && m_selectRect.height() > 0)
    {
        painter.save();
        QPen pen(Qt::green);
        pen.setStyle(Qt::DotLine);
        painter.setPen(pen);
        painter.drawRect(m_selectRect);
        painter.restore();
    }
}

void HFrame::resizeEvent(QResizeEvent * event)
{

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
    return true;
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
    return true;
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

void HFrame::selectObjInRect()
{
    //可以放到selectTool里面
    for(int i = 0; i < pRuleFile->m_drawObjList.count();i++)
    {
        HDrawObj* drawObj = (HDrawObj*)pRuleFile->m_drawObjList[i];
        if(drawObj)
        {
            if(m_selectRect.contains(drawObj->m_rectCurPos))
                select(drawObj,true);
        }
    }

    for(int i = 0; i < pRuleFile->m_connectObjList.count();i++)
    {
        HConnect* conn = (HConnect*)pRuleFile->m_connectObjList[i];
        if(conn)
        {
            if(m_selectRect.contains(conn->m_pointIn) || m_selectRect.contains(conn->m_pointOut))
                selectConnect(conn,true);
        }
    }
}

void HFrame::drawGrid(QPainter &painter)
{
    painter.save();

    QPen pen;
    pen.setColor(Qt::red);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    QRect rect1 = QRect(0,0,pRuleFile->m_Size.width(),pRuleFile->m_Size.height());
    painter.drawLine(rect1.topLeft(),rect1.bottomLeft());
    painter.drawLine(rect1.topLeft(),rect1.topRight());


    /*绘制外框*/
    pen.setColor(QColor(pRuleFile->m_strGridClr));
    pen.setWidth(2);
    painter.setPen(pen);
    int nhBar = 0;
    int nvBar = 0;
    if(m_pScrollArea->horizontalScrollBar())
        nhBar = m_pScrollArea->horizontalScrollBar()->height();
    if(m_pScrollArea->verticalScrollBar())
        nvBar = m_pScrollArea->verticalScrollBar()->width();

    rect1.setTop(rect1.top()+20);
    rect1.setLeft(rect1.left()+10);
    rect1.setWidth(rect1.width()-20-nhBar);
    rect1.setHeight(rect1.height()-20-nvBar);
    painter.drawRect(rect1);

    /*绘制表格*/

    pen.setWidth(1);
    pen.setStyle(Qt::DotLine);
    painter.setPen(pen);
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
    painter.restore();
}

void HFrame::setDrawObjProp(HDrawObj* pObj)
{
    if(pObj->getObjType() == TYPE_LOGICAND || pObj->getObjType() == TYPE_LOGICOR)
    {
        HLogicProp logicProc(pObj);
        logicProc.exec();
    }
    else if(TYPE_INPUT == pObj->getObjType())
    {
        if(((HInputObj*)pObj)->btInputType == TYPE_INPUT_COMP)
        {
            HCompareProp compProp(pObj);
            compProp.m_wStationNo1 = pRuleFile->m_ruleFileData.wStationNo;
            compProp.m_wStationNo2 = pRuleFile->m_ruleFileData.wStationNo;
            compProp.initCompareProp();
            compProp.exec();
        }
        else if(((HInputObj*)pObj)->btInputType == TYPE_INPUT_DIGITAL)
        {
            HDigitalProp digitalPorp(pObj);
            digitalPorp.m_wStationNo = pRuleFile->m_ruleFileData.wStationNo;
            digitalPorp.initDlg();
            if(QDialog::Accepted == digitalPorp.exec())
            {
                ((HInputObj*)pObj)->m_strName = digitalPorp.m_strContent;
                ((HInputObj*)pObj)->m_strRuleName = digitalPorp.m_strFormula;
            }
        }
    }
    update();
}


bool HFrame::isLinkConnectObj(HDrawObj *pObj)
{
    if(pObj == NULL) return false;
    QRect rectDrawObj(QPoint(pObj->m_pointOut.x()-7,pObj->m_pointOut.y() -7),QSize(15,15));
    for(int i = 0; i < pRuleFile->m_connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->m_connectObjList[i];
        QRect rectConnectObj(QPoint(pConnect->m_pLinePoint[0].x()-3,pConnect->m_pLinePoint[0].y() -3),QSize(7,7));
        if(rectDrawObj.intersects(rectConnectObj) || pConnect->m_dwInObjID == pObj->m_dwID)//在矩形框内(初始)连接或者连接线已经连接之后
        {

            pConnect->m_pointIn.setX(pObj->m_pointOut.x());
            pConnect->m_pLinePoint[0].setX(pObj->m_pointOut.x());
            pConnect->m_dwInObjID = pObj->m_dwID;
            if(!isSelect(pConnect))
                pConnect->moveSelectPoint(1,this,pObj->m_pointOut);//移动连接线
        }

        if(pObj->m_nInPointSum > 0)//与 或等结构左边连接
        {
            for(int j =0; j < pObj->m_nInPointSum;j++)
            {
                if(j == pConnect->m_btOutIndex && pConnect->m_dwOutObjID == pObj->m_dwID)
                {
                    pConnect->m_pointOut.setX(pObj->m_pointIn[pConnect->m_btOutIndex].x());
                    pConnect->m_pLinePoint[3].setX(pObj->m_pointIn[pConnect->m_btOutIndex].x());
                    pConnect->m_dwOutObjID = pObj->m_dwID;
                    if(!isSelect(pConnect))
                         pConnect->moveSelectPoint(4,this,pObj->m_pointIn[j]);
                }
                QRect rect(QPoint(pObj->m_pointIn[j].x()-3,pObj->m_pointIn[j].y()-3),QSize(7,7));
                QRect rectConnect(QPoint(pConnect->m_pLinePoint[3].x()-1,pConnect->m_pLinePoint[3].y() -1),QSize(3,3));
                //必须要增加判断当前针脚点是否已经连接
              if(rect.intersects(rectConnect) )
                {
                    if(findConnectInRect(pObj,j,0)/* && pConnect->dwOutObjID == pObj->m_dwID*/)
                    {
                        continue;
                    }
                    pConnect->m_pointOut.setX(pObj->m_pointIn[pConnect->m_btOutIndex].x());
                    pConnect->m_pLinePoint[3].setX(pObj->m_pointIn[j].x());
                    pConnect->m_dwOutObjID = pObj->m_dwID;
                    pConnect->m_btOutIndex = j;
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
    for(int i = 0; i < pRuleFile->m_connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->m_connectObjList[i];
        if(pConnect->m_btOutIndex == wIndex && pConnect->m_dwOutObjID == pObj->m_dwID)//在矩形框内(初始)连接或者连接线已经连接之后
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
    for(int i = 0; i < pRuleFile->m_connectObjList.count();i++)
    {
        HConnect *pConnect = (HConnect*)pRuleFile->m_connectObjList[i];
        if(pConnect->m_dwInObjID == pObj->m_dwID)//在矩形框内(初始)连接或者连接线已经连接之后
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
        if(pConnect->m_dwOutObjID == pObj->m_dwID)
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

void HFrame::delObj()
{
    if(!pRuleFile) return;

    //还要判断是否是连接线
    if(m_selectObjList.count() == 0 && m_selectConnectObjList.count() == 1)
    {
        HConnect *conn = m_selectConnectObjList.takeFirst();
        if(conn)
        {
            pRuleFile->removeConnect(conn);
            delete conn;
            conn = NULL;
        }
    }

    QList<HDrawObj*>::iterator objIt = m_selectObjList.begin();
    for(;objIt != m_selectObjList.end();objIt++)
    {
         HDrawObj* drawObj = (HDrawObj*)*objIt;
         if(!drawObj) return;

         HDrawObj* obj = pRuleFile->findDrawObj(drawObj->m_dwID);
         if(!obj) return;

         //删除连接线
         QList<HConnect*>::iterator it = pRuleFile->m_connectObjList.begin();
         for(;it != pRuleFile->m_connectObjList.end();++it)
         {
             HConnect* conn = (HConnect*)*it;
             if(conn->m_dwInObjID == obj->m_dwID || conn->m_dwOutObjID == obj->m_dwID)
             {
                 pRuleFile->removeConnect(conn);
                 m_selectConnectObjList.removeOne(conn);
                 delete conn;
             }
         }

         //将m_dwID>Obj的m_dwID重新-1编号
         for(int i = 0; i < pRuleFile->m_drawObjList.count();i++)
         {
             HDrawObj* obj1 = (HDrawObj*)pRuleFile->m_drawObjList[i];
             if(obj1->m_dwID < obj->m_dwID) continue;
             quint32 bakID = obj->m_dwID;
             obj1->m_dwID--;
             //在连接线里面找到连接到该对象的元素，然后ID也要-1
             for(int k = 0; k < pRuleFile->m_connectObjList.count();k++)
             {
                 HConnect* conn1 = (HConnect*)pRuleFile->m_connectObjList[k];
                 if(conn1->m_dwInObjID == bakID)
                     conn1->m_dwInObjID = obj1->m_dwID;
                 else if(conn1->m_dwOutObjID == bakID)
                     conn1->m_dwOutObjID = obj1->m_dwID;
             }
         }

         pRuleFile->removeObj(obj);
         m_selectObjList.removeOne(drawObj);
         delete obj;
         drawObj = NULL;
         obj = NULL;
    }

    //重置ID
    pRuleFile->refreshObjID();
    this->update();

}

void HFrame::cutObj()
{
    copyObj();
    delObj();
}

void HFrame::copyObj()
{
    QByteArray bytes;
    QDataStream stream(&bytes,QIODevice::WriteOnly);
    //写入stream
    stream<<m_selectObjList.count();
    QList<HDrawObj*>::iterator it = m_selectObjList.begin();
    for(;it != m_selectObjList.end();++it)
    {
        HDrawObj* pObj = (HDrawObj*)(*it);
        if(pObj)
        {
            stream<<pObj->m_btObjType;
            pObj->writeData(QDataStream::Qt_5_7,&stream);
        }
    }

    stream<<m_selectConnectObjList.count();
    QList<HConnect*>::iterator itc = m_selectConnectObjList.begin();
    for(;itc != m_selectConnectObjList.end();++itc)
    {
        HConnect* pObj = (HConnect*)(*itc);
        if(pObj)
        {
            pObj->writeData(QDataStream::Qt_5_7,&stream);
        }
    }

    QString clipboardPath = getClipboardFile();
    QFile file(clipboardPath);
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream cbStream(&file);
        cbStream.writeBytes(bytes.data(),bytes.length());
        file.close();
    }
}

void HFrame::pasteObj()
{
    QString clipboardPath = getClipboardFile();
    QFile file(clipboardPath);
    if(!file.exists() || !file.open(QIODevice::ReadOnly))
        return;
    QDataStream stream(&file);
    int drawObjCount;
    stream>>drawObjCount;
    if(drawObjCount == 0) return;
    QPoint point = QPoint(0,0);
    for(int i = 0; i < drawObjCount;i++)
    {
        quint8 btObjType;
        stream>>btObjType;
        HDrawObj* pObj = NULL;
        if(TYPE_INPUT == btObjType)
        {
            pObj = new HInputObj(QRect(point,QSize(100,40)),pRuleFile);
        }
        else if(TYPE_LOGICAND == btObjType)
        {
            pObj = new HAndObj(QRect(point,QSize(80,84)),pRuleFile);
        }
        else if(TYPE_LOGICOR == btObjType)
        {
            pObj = new HOrObj(QRect(point,QSize(80,84)),pRuleFile);
        }
        else if(TYPE_RESULT == btObjType)
        {
            pObj = new HResultObj(QRect(point,QSize(100,40)),pRuleFile);
        }
        else
            return;

        pObj->readData(QDataStream::Qt_5_7,&stream);
        pRuleFile->addObject(pObj);
    }

    //连接线
    int connObjCount;
    stream>>connObjCount;
    if(connObjCount == 0) return;
    for(int i = 0; i < connObjCount;i++)
    {
        HConnect* conn = new HConnect();
        conn->readData(QDataStream::Qt_5_7,&stream);
        pRuleFile->addConnect(conn);
    }
    file.close();
    //不能删除文件可能需要重复粘贴
    //更新一下拷贝之后的信息
    updatePasteObj();
}

QString HFrame::getClipboardFile()
{
    QString clipboardPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    clipboardPath.append("/temp/rule");
    QDir dir(clipboardPath);
    if(!dir.exists())
        dir.mkdir(clipboardPath);
    clipboardPath.append("/ruleclipboard.tmp");
    return clipboardPath;
}

void HFrame::updatePasteObj()
{
    if(!pRuleFile) return;
    pRuleFile->refreshObjID();

    //将选择的部分变成新增的部分
    for(int i = 0; i < m_selectObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)m_selectObjList[i];
        if(pObj)
        {
            pObj->m_pRuleFile = pRuleFile;
            pObj->saveOldID();
            pObj->generateID();
            pObj->setWrongFlag(false);
            pObj->setOffset();
        }
    }

    //更新连接线
    /*将连接线上oldID*/
    for(int i = 0; i < m_selectConnectObjList.count();i++)
    {
        HConnect* conn = (HConnect*)m_selectConnectObjList[i];
        bool bConnectIn,bConnectOut;
        bConnectIn = bConnectOut = false;
        for(int k = 0; k < m_selectObjList.count();k++)
        {
            HDrawObj* drawObj = (HDrawObj*)m_selectObjList[i];
            if(drawObj && conn->m_dwInObjID == drawObj->dwOldID && !bConnectIn)
            {
                conn->m_dwInObjID = drawObj->m_dwID;
                bConnectIn = true;
            }
            if(drawObj && conn->m_dwOutObjID == drawObj->dwOldID && !bConnectOut)
            {
                conn->m_dwOutObjID = drawObj->m_dwID;
                bConnectOut = true;
            }
        }
        if(bConnectIn && bConnectOut)
        {
            conn->setOffSet();
            conn->m_pRuleFile = pRuleFile;
        }
    }
    update();
}




