#include "rulefile.h"
#include "ruleeditapi.h"
HRuleFile::HRuleFile(QObject *parent) : QObject(parent)
{
    dwDrawObjID = 0;
    m_bGrid = true;
    m_strBgClr = QColor(Qt::white).name();
    m_strGridClr = QColor(0,0,128).name();
    m_strFillClr = QColor(206,230,255).name(); //填充色
    m_strLineClr = QColor(0,128,128).name(); //线条颜色
    //m_clrText = QColor(0,0,255).name(); //文字颜色
    m_strUpedgeClr = QColor(255,255,255).name(); //上边框颜色
    m_strDownedgeClr = QColor(0,0,0).name(); //下边框颜色
    m_strShadowClr = QColor(128,128,128).name(); //阴影颜色
    bDisplayID = false;//
    bSimulateFirst = false;
    bSimuState = false;
    m_Size = QSize(1200,1000);
}


QDataStream& operator>>(QDataStream& in,HRuleFile& ruleFile)
{
   return in;
}

void HRuleFile::add(HDrawObj* pObj)
{
    if(drawObjList.contains(pObj) == false)
    {
        drawObjList.append(pObj);
    }
}

void HRuleFile::addConnect(HConnect* pObj)
{
    if(connectObjList.contains(pObj) == false)
    {
        connectObjList.append(pObj);
    }
}

HDrawObj* HRuleFile::findDrawObj(quint32 ulID)
{
    HDrawObj* pObj = NULL;
    for(int i =0; i < drawObjList.count();i++)
    {
        pObj = (HDrawObj*)drawObjList[i];
        if(pObj->dwID == ulID)
            return pObj;
    }

    return NULL;
}

HDrawObj* HRuleFile::objectAt(const QPoint &point)
{
	QRect rect(point, QSize(1, 1));
	for (int i = 0; i < drawObjList.count(); i++)
	{
		HDrawObj* pObj = (HDrawObj*)drawObjList[i];
		if (pObj->m_rectCurPos.intersects(rect))
			return pObj;
	}
	return NULL;
}

HConnect* HRuleFile::connectAt(const QPoint& point)
{
    QRect rect(point, QSize(2, 2));
	for (int i = 0; i < connectObjList.count(); i++)
	{
		HConnect* pObj = (HConnect*)connectObjList[i];
		if (pObj->intersects(rect))
			return pObj;
	}
	return NULL;
}

HResultObj* HRuleFile::resultObj()
{
	for (int i = 0; i < drawObjList.count(); i++)
	{
		HDrawObj* pObj = (HDrawObj*)drawObjList[i];
		if (pObj != NULL && pObj->m_btObjType == TYPE_RESULT)
			return (HResultObj*)pObj;

		/*  if(pObj == NULL)
			  return false;
			  const QMetaObject *pMObj = pObj->metaObject();;
			  //qDebug()<<"className:"<<pMObj->className();
			  if(QString(pMObj->superClass()->className()).compare("HDrawObj") == 0)
			  return m_selectObjList.contains((HDrawObj*)pObj);
			  if(QString(pMObj->className()).compare("HConnect") == 0)
			  return m_selectConnectObjList.contains((HConnect*)pObj);*/

	}
	return NULL;
}

bool HRuleFile::isObjConnect(HDrawObj *pDrawObj)
{
    quint32 dwObjID = pDrawObj->m_pRuleFile->dwDrawObjID;
	if (pDrawObj->m_nInPointSum > 0 || pDrawObj->m_nOutPointSum > 0)
	{
		for (int i = 0; i < connectObjList.count(); i++)
		{
			HConnect* pConnect = (HConnect*)connectObjList[i];
			if (!pConnect) continue;
			if (pConnect->dwInObjID == dwObjID || pConnect->dwOutObjID == dwObjID)
				return true;
		}
		return false;
	}
	return true;
}

HResultObj* HRuleFile::getResultObj()
{
	HResultObj* pObj = NULL;
	for (int i = 0; i < drawObjList.count(); i++)
	{
		HDrawObj* obj = drawObjList[i];
		if (obj != NULL && obj->getObjType() == TYPE_RESULT)
		{
			pObj = (HResultObj*)obj;
			break;
		}
	}
	return pObj;
}

HDrawObj* HRuleFile::getConnectObj(HDrawObj* target, int nConnNo)
{
    quint32 dwTargetObjID;
	quint16 dwConnectedID;

	dwTargetObjID = target->dwID;
	for (int i = connectObjList.count(); i > 0; i--)
	{
		HConnect* conn = connectObjList[i];
		if (conn && conn->dwOutObjID == dwTargetObjID && conn->btOutIndex == nConnNo)
		{
			dwConnectedID = conn->dwInObjID;
			HDrawObj* pConnObj = findDrawObj(dwConnectedID);
			if (!pConnObj) return NULL;
			if (target->getObjType() == TYPE_RESULT)
			{
				if (pConnObj->getObjType() == TYPE_RESULT)
					return NULL;
				else
					return pConnObj;
			}
			else if (target->getObjType() == TYPE_LOGICOR || target->getObjType() == TYPE_LOGICAND)
			{
				if (pConnObj->getObjType() == TYPE_LOGICOR || pConnObj->getObjType() == TYPE_LOGICAND)
			    {
					return pConnObj;
                }
                else if (pConnObj->getObjType() == TYPE_INPUT)
				{
					return pConnObj;
				}
				else
					return NULL;
			}
        }
    }
    return NULL;
}

void HRuleFile::removeObj(HDrawObj* drawObj)
{
    drawObjList.removeOne(drawObj);
}

void HRuleFile::removeConnect(HConnect* connectObj)
{
    connectObjList.removeOne(connectObj);
}

//////////////////////////////////////////////////////////////////HPointRule////////////////////////////////////////////////////
HPointRule::HPointRule()
{

}

HPointRule::~HPointRule()
{

}

QDataStream& operator>>(QDataStream& in,HPointRule& pProtRule)
{
    return in;
}

QDataStream& operator<<(QDataStream& out,const HPointRule& pProtRule)
{
    return out;
}



////////////////////////////////////////////////////////////////HProtectRule///////////////////////////////////////////////////
HProtectRule::HProtectRule()
{

}

HProtectRule::~HProtectRule()
{

}

QDataStream& operator>>(QDataStream& in,HProtectRule& pProtRule)
{
    return in;
}

QDataStream& operator<<(QDataStream& out,const HProtectRule& pProtRule)
{
    return out;
}

bool HProtectRule::addPointRule(HPointRule* pPointRule)//增加点规则
{
    pPointRuleList.append(pPointRule);
    return true;
}

HPointRule* HProtectRule::pointRule(quint8 btType,quint16 wPointID)//获取规则
{
    HPointRule* pPointRule;
    for(int i =0; i < pPointRuleList.count();i++)
    {
        pPointRule = (HPointRule*)pPointRuleList[i];
        if(pPointRule->wPointType == btType && pPointRule->wPointNo == wPointID)
            return pPointRule;
    }
    return NULL;
}

HPointRule* HProtectRule::findPointRule(int nIndex)
{
    if(pPointRuleList.count() < 0 || pPointRuleList.count() < nIndex)
        return NULL;
    return pPointRuleList[nIndex];
}

//int pointRuleCount(){return pPointRuleList.count();}
bool HProtectRule::delPointRuleByID(quint8 btType,int wPointID)
{
    HPointRule *pPointRule = NULL;
    for(int i = 0; i < pPointRuleList.count(); i++)
    {
        pPointRule = (HPointRule*)pPointRuleList[i];
        if(pPointRule->wPointType == btType && pPointRule->wPointNo == wPointID)
        {
            pPointRuleList.removeOne(pPointRule);
            delete pPointRule;
            return true;
        }
    }
    return false;
}

bool HProtectRule::addRuleFile(HRuleFile* pRuleFile)
{
    if(pRuleFileList.contains(pRuleFile))
        return false;
    pRuleFileList.append(pRuleFile);
    return true;
}

HRuleFile* HProtectRule::ruleFile(int wType,int wPointNo,int wRelayValue)
{
    HPointRule* pPointRule = pointRule(wType,wPointNo);
    quint16 wRuleFileID;
    if(wRelayValue == RELAY_OPEN)
        wRuleFileID = pPointRule->wOpenRuleFileID;
    else if(wRelayValue == RELAY_CLOSE)
        wRuleFileID = pPointRule->wCloseRuleFileID;
    else if(wRelayValue == RELAY_JXOPEN)
        wRuleFileID = pPointRule->wJXOpenRuleFileID;
    else if(wRelayValue == RELAY_JXCLOSE)
        wRuleFileID = pPointRule->wJXCloseRuleFileID;
    else
        wRuleFileID = 0;

    HRuleFile *pRuleFile = getRuleFileByID(wRuleFileID);
    if(!pRuleFile) return NULL;
    return pRuleFile;

}

HRuleFile* HProtectRule::findRuleFile(int nIndex)
{
    if(nIndex < 0 || nIndex > pRuleFileList.count())
        return NULL;
    return pRuleFileList[nIndex];
}

HRuleFile* HProtectRule::getRuleFileByID(quint16 wRuleFileID)
{
    HRuleFile *pRuleFile = NULL;
    for(int i = 0; i < pRuleFileList.count();i++)
    {
        pRuleFile = (HRuleFile*)pRuleFileList[i];
        if(pRuleFile->nRuleFileID == (int)wRuleFileID)
            return pRuleFile;
    }
    return NULL;
}

//int ruleFileCount(){return pRuleFileList.count();}
bool HProtectRule::delRuleFileByID(int wRuleFileID)
{
    HRuleFile* pRuleFile = NULL;
    for(int i = 0; i < pRuleFileList.count();i++)
    {
        pRuleFile = (HRuleFile*)pRuleFileList[i];
        if(pRuleFile->nRuleFileID == wRuleFileID)
        {
            pRuleFileList.removeOne(pRuleFile);
            delete pRuleFile;
            return true;
        }
    }
    return false;
}

bool HProtectRule::delRuleFile(int wType,int wPointNo,int wRelayValue)
{
    HPointRule *pPointRule = pointRule(wType,wPointNo);
    quint16 wRuleFileID;
    if(wRelayValue == RELAY_OPEN)
        wRuleFileID = pPointRule->wOpenRuleFileID;
    else if(wRelayValue == RELAY_CLOSE)
        wRuleFileID = pPointRule->wCloseRuleFileID;
    else if(wRelayValue == RELAY_JXOPEN)
        wRuleFileID = pPointRule->wJXOpenRuleFileID;
    else if(wRelayValue == RELAY_JXCLOSE)
        wRuleFileID = pPointRule->wJXCloseRuleFileID;

    if(delRuleFileByID(wRuleFileID))
        return true;
    return false;
}


//////////////////////////////////////////////HStationRule////////////////////////////////
HStationRule::HStationRule()
{

}

HStationRule::~HStationRule()
{

}

void HStationRule::maxRuleFileID()
{
    wRuleFileID = 0;
}

bool HStationRule::addProtectRule(HProtectRule* pProtRule)
{
    pProtRuleList.append(pProtRule);
    return true;
}

QDataStream& operator>>(QDataStream& in,HStationRule& stationRule)
{
    int qtVersion;
    in>>qtVersion;
    in>>stationRule.wStationNo;
    in>>stationRule.wRuleFileID;
    QString strTemp1,strTemp2;
    in>>strTemp1;
    in>>strTemp2;
    qstrcpy(stationRule.szStationName,strTemp1.toLocal8Bit().data());
    qstrcpy(stationRule.szRuleFileName,strTemp2.toLocal8Bit().data());
    //QList<HProtectRule*> pProtRuleList;
    int nCount = 0;
    in>>nCount;
    for(int i = 0; i < nCount;i++)
    {
        HProtectRule* pProtRule = new HProtectRule;
        in>>(*pProtRule);
        stationRule.pProtRuleList.append(pProtRule);
    }
    return in;
}

QDataStream& operator<<(QDataStream& out,const HStationRule& stationRule)
{
    out<<QDataStream::Qt_5_2;
    out<<stationRule.wStationNo;
    out<<stationRule.wRuleFileID;
    QString strTemp1 = QString("%1").arg(stationRule.szStationName);
    out<<strTemp1;
    QString strTemp2 = QString("%1").arg(stationRule.szRuleFileName);
    out<<strTemp2;
    out<<stationRule.pProtRuleList.count();
    for(int i = 0;i<stationRule.pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)stationRule.pProtRuleList[i];
        out<<(*pProtRule);
    }
    return out;
}

HProtectRule* HStationRule::protectRule(quint16 wProtNo)
{
    HProtectRule* pProtRule = NULL;
    for(int i = 0; i < pProtRuleList.count();i++)
    {
        pProtRule = (HProtectRule*)pProtRuleList[i];
        if(pProtRule->wProtectNo == wProtNo)
            return pProtRule;
    }
    return NULL;
}

HRuleFile* HStationRule::ruleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue)
{
    HRuleFile* pRuleFile = NULL;
    for(int i = 0; i < pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)pProtRuleList[i];
        if(pProtRule)
        {
            pRuleFile = pProtRule->ruleFile(wType,wPointNo,wRelayValue);
            if(!pRuleFile)
                return pRuleFile;
        }
    }
    return NULL;
}

bool HStationRule::delRuleFile(quint16 wType,quint16 wPointNo,quint16 wRelayValue)
{
    for(int i = 0; i < pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)pProtRuleList[i];
        if(pProtRule)
        {
            if(pProtRule->delRuleFile(wType,wPointNo,wRelayValue))
                return true;
        }
    }
    return false;
}

bool HStationRule::delPointRule(quint16 wType,quint16 wPointNo)
{
    for(int i = 0; i < pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)pProtRuleList[i];
        if(pProtRule)
        {
            if(pProtRule->delPointRuleByID(wType,wPointNo))
                return true;
        }
    }
    return false;
}

bool HStationRule::changeStationNo(quint16 wNewStationNo)
{
    wStationNo = wNewStationNo;
    for(int i = 0; i < pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)pProtRuleList[i];
        if(pProtRule)
        {
            pProtRule->wStationNo = wStationNo;
            //还要刷新装置下的测点和规则文件
        }
    }
    return false;
}




///////////////////////////////遍历函数部分////////////////////////////////////////////////////
//遍历主要是从结果往前推，而不是从前往后推
bool HRuleFile::buildGeneralFormula()
{
     bFormulaRight = true;
    strFormula = "";
    //遍历所有对象，是否存在未连接对象，同时设置遍历标记
    for(int i = 0; i < drawObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)drawObjList[i];
        if(pObj)
        {
            pObj->setVisiteFlag(false);
            if(!isObjConnect(pObj))
                pObj->setWrongFlag(true);
            else
                pObj->setWrongFlag(false);
        }
    }

    //找到最后输出对象
    HResultObj* pResultObj = getResultObj();
    if(!pResultObj) //没有输出对象则公式全部错误
    {
        for(int i = 0; i < drawObjList.count();i++)
        {
            HDrawObj* pObj = (HDrawObj*)drawObjList[i];
            if(pObj)
            {
                    pObj->setWrongFlag(false);
            }
        }
        return false;
    }
    pResultObj->setVisiteFlag(true);

    //从输出对象开始找到第一个连接对象
	HDrawObj* pFirstObj = getConnectObj(pResultObj, 0);
	if (!pFirstObj)
    {
        pResultObj->setWrongFlag(true);
        if(!visitGeneralBuildObj(pFirstObj))
        {
            bFormulaRight = false;
        }
    }
	else
	{
        pResultObj->setWrongFlag(false);
        bFormulaRight = false;
	}
    if (!bFormulaRight)
		return false;
	if (strFormula.isEmpty())
		return false;

    return true;
}

bool HRuleFile::visitGeneralBuildObj(HDrawObj* pObj)
{
	int nOpCount;
	bool bErrorFlag = true;
    if (pObj->getObjType() == TYPE_INPUT)
	{
		strFormula = strFormula + pObj->getOperatorFirst();
		strFormula = strFormula + pObj->getOperatorMid();
		strFormula = strFormula + pObj->getOperatorLast();
	}
	else //(pObj->getObjType == TYPE_LOGICAND)
	{
		strFormula = strFormula + pObj->getOperatorFirst();
		for (nOpCount = 0; nOpCount < pObj->m_nInPointSum; nOpCount++)
		{
			HDrawObj* pConnObj = getConnectObj(pObj, nOpCount);
			if (pConnObj)
			{
				bErrorFlag = visitGeneralBuildObj(pConnObj);
				if (nOpCount < pObj->m_nInPointSum - 1)
					strFormula = strFormula + pObj->getOperatorMid();
			}
			else
			{
				pObj->setWrongFlag(true);
                bFormulaRight = false;
			}
		}
		strFormula += pObj->getOperatorLast();
	}
	pObj->setVisiteFlag(true);
	return bErrorFlag;
}

///////////////////////////////////////////////////////////仿真遍历函数//////////////////////////////////////////////////////
void HRuleFile::buildSimulateFormula()
{
    //首先获取唯一输出对象
    HResultObj* pResultObj = getResultObj();
    if(!pResultObj) return;
    //然后获取和输出对象连接的
    quint32 nConnObjID = 0; //连接对象ID
    quint32 nResultObjID = pResultObj->dwID;
    QList<HConnect*>::iterator it = connectObjList.begin();
    for(;it != connectObjList.end();++it)
    {
       HConnect *conn = (HConnect*)(*it);
       if(conn->dwOutObjID == nResultObjID)
       {
           nConnObjID = conn->dwInObjID;
           HDrawObj* drawObj = (HDrawObj*)findDrawObj(nConnObjID);
           if(drawObj)
           {
               return;
           }
           if(TYPE_LOGICAND == drawObj->getObjType())
           {
               visitSimulateBuildObj(drawObj);
               pResultObj->m_bOutValue = drawObj->m_bOutValue;
           }
           else if(TYPE_LOGICOR == drawObj->getObjType())
           {
               visitSimulateBuildObj(drawObj);
               pResultObj->m_bOutValue = drawObj->m_bOutValue;
           }
           else if(TYPE_INPUT == drawObj->getObjType())
           {
               pResultObj->m_bOutValue = drawObj->m_bOutValue;
           }
       }
    }
}

void HRuleFile::visitSimulateBuildObj(HDrawObj* drawObj)
{
    if(!drawObj)
        return;
    if(TYPE_LOGICAND == drawObj->getObjType() || TYPE_LOGICOR == drawObj->getObjType())
    {
        for(int i = 0; i < drawObj->m_nInPointSum;i++)
        {
            HDrawObj* obj = getConnectObj(drawObj,i);
            if(obj &&(TYPE_LOGICAND == obj->getObjType() || TYPE_LOGICOR == obj->getObjType()))
            {
                visitSimulateBuildObj(obj);
                drawObj->addInValue(i,obj->outValue());
            }
            else if(obj && TYPE_INPUT == obj->getObjType())
            {
                drawObj->addInValue(i,obj->outValue());
            }
        }

        //最后输出值
        drawObj->outValue();
    }
}
