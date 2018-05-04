#include "rulefile.h"
#include "ruleeditapi.h"
#include <QList>
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;
HRuleFile::HRuleFile(QObject *parent) : QObject(parent)
{
    m_wDrawObjID = 1;
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

HRuleFile::~HRuleFile()
{
    clear();
}

void HRuleFile::clear()
{
    while(!m_drawObjList.isEmpty())
        delete (HDrawObj*)m_drawObjList.takeFirst();
    m_drawObjList.clear();

    while(!m_connectObjList.isEmpty())
        delete (HConnect*)m_connectObjList.takeFirst();
    m_connectObjList.clear();
}

void HRuleFile::readData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    quint16 n16;
    *ds>>n16;
    m_wRuleFileID = n16;
    QString s;
    *ds>>s;
    m_strRuleName = s;
    *ds>>s;
    m_strFormula = s;
    bool b;
    *ds>>b;
    m_bGrid = b;
    *ds>>s;
    m_strBgClr = s;
    *ds>>s;
    m_strGridClr = s;
    *ds>>s;
    m_strFillClr = s;
    *ds>>s;
    m_strUpedgeClr = s;
    *ds>>s;
    m_strDownedgeClr = s;
    *ds>>s;
    m_strShadowClr = s;

    int nConnectObjCount;
    int n;
    *ds>>n;
    nConnectObjCount = n;
    for(int i = 0; i < nConnectObjCount;i++)
    {
        HConnect* conn = (HConnect*)m_connectObjList[i];
        if(conn)
        {
            conn->readData(nVersion,ds);
        }
    }
    int nDrawObjCount;
    *ds >> n;
    nDrawObjCount = n;
    for(int i = 0; i < nDrawObjCount;i++)
    {
        HDrawObj* drawObj = (HDrawObj*)m_drawObjList[i];
        if(drawObj)
        {
            drawObj->readData(nVersion,ds);
        }
    }
}

void HRuleFile::writeData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    *ds<<(quint16)m_wRuleFileID;
    *ds<<(QString)m_strRuleName;
    *ds<<(QString)m_strFormula;
    *ds<<(bool)m_bGrid;
    *ds<<(QString)m_strBgClr;
    *ds<<(QString)m_strGridClr;
    *ds<<(QString)m_strFillClr;
    *ds<<(QString)m_strLineClr;
    *ds<<(QString)m_strUpedgeClr;
    *ds<<(QString)m_strDownedgeClr;
    *ds<<(QString)m_strShadowClr;
    int n = m_connectObjList.count();
    *ds<<n;
    for(int i = 0; i < n;i++)
    {
        HConnect* conn = (HConnect*)m_connectObjList[i];
        if(conn)
            conn->writeData(nVersion,ds);
    }
    n = m_drawObjList.count();
    *ds<<n;
    for(int i = 0; i < n;i++)
    {
        HDrawObj* drawObj = (HDrawObj*)m_drawObjList[i];
        if(drawObj)
            drawObj->writeData(nVersion,ds);
    }
}


void HRuleFile::add(HDrawObj* pObj)
{
    if(m_drawObjList.contains(pObj) == false)
    {
        m_drawObjList.append(pObj);
    }
}

void HRuleFile::addConnect(HConnect* pObj)
{
    if(m_connectObjList.contains(pObj) == false)
    {
        m_connectObjList.append(pObj);
    }
}

HDrawObj* HRuleFile::findDrawObj(quint32 ulID)
{
    HDrawObj* pObj = NULL;
    for(int i =0; i < m_drawObjList.count();i++)
    {
        pObj = (HDrawObj*)m_drawObjList[i];
        if(pObj->m_dwID == ulID)
            return pObj;
    }

    return NULL;
}

HDrawObj* HRuleFile::objectAt(const QPoint &point)
{
	QRect rect(point, QSize(1, 1));
    for (int i = 0; i < m_drawObjList.count(); i++)
	{
        HDrawObj* pObj = (HDrawObj*)m_drawObjList[i];
		if (pObj->m_rectCurPos.intersects(rect))
			return pObj;
	}
	return NULL;
}

HConnect* HRuleFile::connectAt(const QPoint& point)
{
    QRect rect(point, QSize(2, 2));
    for (int i = 0; i < m_connectObjList.count(); i++)
	{
        HConnect* pObj = (HConnect*)m_connectObjList[i];
		if (pObj->intersects(rect))
			return pObj;
	}
	return NULL;
}

HResultObj* HRuleFile::resultObj()
{
    for (int i = 0; i < m_drawObjList.count(); i++)
	{
        HDrawObj* pObj = (HDrawObj*)m_drawObjList[i];
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
    quint32 dwObjID = pDrawObj->m_pRuleFile->m_wDrawObjID;
	if (pDrawObj->m_nInPointSum > 0 || pDrawObj->m_nOutPointSum > 0)
	{
        for (int i = 0; i < m_connectObjList.count(); i++)
		{
            HConnect* pConnect = (HConnect*)m_connectObjList[i];
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
    for (int i = 0; i < m_drawObjList.count(); i++)
	{
        HDrawObj* obj = m_drawObjList[i];
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

    dwTargetObjID = target->m_dwID;
    for (int i = m_connectObjList.count(); i > 0; i--)
	{
        HConnect* conn = m_connectObjList[i];
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
    m_drawObjList.removeOne(drawObj);
}

void HRuleFile::removeConnect(HConnect* connectObj)
{
    m_connectObjList.removeOne(connectObj);
}

//////////////////////////////////////////////////////////////////HPointRule////////////////////////////////////////////////////
HPointRule::HPointRule()
{

}

HPointRule::~HPointRule()
{

}

void HPointRule::readData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    quint16 n16;
    *ds>>n16;
    m_wStationNo = n16;
    *ds>>n16;
    m_wProtectNo = n16;
    *ds>>n16;
    m_wPointNo = n16;
    quint8 n8;
    *ds>>n8;
    m_wPointType = n8;
    *ds>>n8;
    m_btInsideType = n8;
    *ds>>n16;
    m_wOpenFormulaID = n16;
    *ds>>n16;
    m_wCloseFormulaID = n16;
    *ds>>n16;
    m_wJXOpenFormulaID = n16;
    *ds>>n16;
    m_wJXCloseFormulaID = n16;
    *ds>>n16;
    m_wOpenRuleFileID = n16;
    *ds>>n16;
    m_wCloseRuleFileID = n16;
    *ds>>n16;
    m_wJXOpenRuleFileID = n16;
    *ds>>n16;
    m_wJXCloseRuleFileID = n16;
    QString s;
    *ds>>s;
    m_strPointName = s;;
}

void HPointRule::writeData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    *ds<<(quint16) m_wStationNo;
    *ds<<(quint16) m_wProtectNo;
    *ds<<(quint16) m_wPointNo;
    *ds<<(quint8) m_wPointType;
    *ds<<(quint8)  m_btInsideType;
    *ds<<(quint16) m_wOpenFormulaID;
    *ds<<(quint16) m_wCloseFormulaID;
    *ds<<(quint16) m_wJXOpenFormulaID;
    *ds<<(quint16) m_wJXCloseFormulaID;
    *ds<<(quint16) m_wOpenRuleFileID;
    *ds<<(quint16) m_wCloseRuleFileID;
    *ds<<(quint16) m_wJXOpenRuleFileID;
    *ds<<(quint16) m_wJXCloseRuleFileID;
    *ds<<(QString) m_strPointName;
}



////////////////////////////////////////////////////////////////HProtectRule///////////////////////////////////////////////////
HProtectRule::HProtectRule()
{
    m_wStationNo = (quint16)-1;
    m_wProtectNo = (quint16)-1;
    m_pPointRuleList.clear();
    m_pRuleFileList.clear();
}

HProtectRule::~HProtectRule()
{
    clear();
}

void HProtectRule::clear()
{
    while(!m_pPointRuleList.isEmpty())
        delete (HPointRule*)m_pPointRuleList.takeFirst();
    m_pPointRuleList.clear();

    while(!m_pRuleFileList)
        delete (HRuleFile*)m_pRuleFileList.takeFirst();
    m_pRuleFileList.clear();
    //QList<HRuleFile*> m_pRuleFileList; //每个点都有一个规则，多少个点就有多少个规则

}

void HProtectRule::readData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    quint16 n16;
    *ds>>n16;
    m_wStationNo = n16;
    *ds>>n16;
    m_wProtectNo = n16;
    QString s;
    *ds>>s;
    m_strDeviceName = s;
    quint16 wPtRuleCount;
    *ds>>n16;
    wPtRuleCount = n16;
    for(int i = 0 ; i < wPtRuleCount;i++)
    {
        HPointRule* ptRule = new HPointRule;
        ptRule->readData(nVersion,ds);
    }
    quint16 wRuleFileCount;
    *ds>>n16;
    wRuleFileCount = n16;
    for(int i = 0; i < wRuleFileCount;i++)
    {
        HRuleFile* rf = new HRuleFile;
        rf->readData(nVersion,ds);
    }
}
void HProtectRule::writeData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    *ds<<(quint16)m_wStationNo;
    *ds<<(quint16)m_wProtectNo;
    *ds<<(QString)m_strDeviceName;
    quint16 wPtRuleCount = m_pPointRuleList.count();
    *ds<<wPtRuleCount;
    for(int i = 0; i < wPtRuleCount;i++)
    {
        HPointRule* ptRule = (HPointRule*)m_pPointRuleList[i];
        if(ptRule)
            ptRule->writeData(nVersion,ds);
    }
    quint16 wRuleFileCount = m_pRuleFileList.count();
    *ds<<wRuleFileCount;
    for(int i = 0; i < wRuleFileCount;i++)
    {
        HRuleFile* rf = (HRuleFile*)m_pRuleFileList[i];
        if(rf)
            rf->writeData(nVersion,ds);
    }
}

bool HProtectRule::addPointRule(HPointRule* pPointRule)//增加点规则
{
    m_pPointRuleList.append(pPointRule);
    return true;
}

HPointRule* HProtectRule::pointRule(quint8 btType,quint16 wPointID)//获取规则
{
    HPointRule* pPointRule;
    for(int i =0; i < m_pPointRuleList.count();i++)
    {
        pPointRule = (HPointRule*)m_pPointRuleList[i];
        if(pPointRule->m_wPointType == btType && pPointRule->m_wPointNo == wPointID)
            return pPointRule;
    }
    return NULL;
}

HPointRule* HProtectRule::findPointRule(int nIndex)
{
    if(m_pPointRuleList.count() < 0 || m_pPointRuleList.count() < nIndex)
        return NULL;
    return m_pPointRuleList[nIndex];
}

//int pointRuleCount(){return pPointRuleList.count();}
bool HProtectRule::delPointRuleByID(quint8 btType,int wPointID)
{
    HPointRule *pPointRule = NULL;
    for(int i = 0; i < m_pPointRuleList.count(); i++)
    {
        pPointRule = (HPointRule*)m_pPointRuleList[i];
        if(pPointRule->m_wPointType == btType && pPointRule->m_wPointNo == wPointID)
        {
            m_pPointRuleList.removeOne(pPointRule);
            delete pPointRule;
            return true;
        }
    }
    return false;
}

bool HProtectRule::addRuleFile(HRuleFile* pRuleFile)
{
    if(m_pRuleFileList.contains(pRuleFile))
        return false;
    m_pRuleFileList.append(pRuleFile);
    return true;
}

HRuleFile* HProtectRule::ruleFile(int wType,int wPointNo,quint8 btYKType)
{
    HPointRule* pPointRule = pointRule(wType,wPointNo);
    quint16 wRuleFileID;
    if(btYKType == CTRL_OPEN)
        wRuleFileID = pPointRule->m_wOpenRuleFileID;
    else if(btYKType == CTRL_CLOSE)
        wRuleFileID = pPointRule->m_wCloseRuleFileID;
    else if(btYKType == CTRL_JXOPEN)
        wRuleFileID = pPointRule->m_wJXOpenRuleFileID;
    else if(btYKType == CTRL_JXCLOSE)
        wRuleFileID = pPointRule->m_wJXCloseRuleFileID;
    else
        wRuleFileID = 0;

    HRuleFile *pRuleFile = getRuleFileByID(wRuleFileID);
    if(!pRuleFile) return NULL;
    return pRuleFile;

}

HRuleFile* HProtectRule::findRuleFile(int nIndex)
{
    if(nIndex < 0 || nIndex > m_pRuleFileList.count())
        return NULL;
    return m_pRuleFileList[nIndex];
}

HRuleFile* HProtectRule::getRuleFileByID(quint16 wRuleFileID)
{
    HRuleFile *pRuleFile = NULL;
    for(int i = 0; i < m_pRuleFileList.count();i++)
    {
        pRuleFile = (HRuleFile*)m_pRuleFileList[i];
        if(pRuleFile->m_wRuleFileID == wRuleFileID)
            return pRuleFile;
    }
    return NULL;
}

//int ruleFileCount(){return pRuleFileList.count();}
bool HProtectRule::delRuleFileByID(int wRuleFileID)
{
    HRuleFile* pRuleFile = NULL;
    for(int i = 0; i < m_pRuleFileList.count();i++)
    {
        pRuleFile = (HRuleFile*)m_pRuleFileList[i];
        if(pRuleFile->m_wRuleFileID == (quint16)wRuleFileID)
        {
            m_pRuleFileList.removeOne(pRuleFile);
            delete pRuleFile;
            return true;
        }
    }
    return false;
}

bool HProtectRule::delRuleFile(int wType,int wPointNo,quint8 btYKType)
{
    HPointRule *pPointRule = pointRule(wType,wPointNo);
    quint16 wRuleFileID;
    if(btYKType == CTRL_OPEN)
        wRuleFileID = pPointRule->m_wOpenRuleFileID;
    else if(btYKType == CTRL_CLOSE)
        wRuleFileID = pPointRule->m_wCloseRuleFileID;
    else if(btYKType == CTRL_JXOPEN)
        wRuleFileID = pPointRule->m_wJXOpenRuleFileID;
    else if(btYKType == CTRL_JXCLOSE)
        wRuleFileID = pPointRule->m_wJXCloseRuleFileID;

    if(delRuleFileByID(wRuleFileID))
        return true;
    return false;
}

//寻找当前ID是否存在
void HProtectRule::refreshRuleFileID(HPointRule *ptRule)
{
    QList<HPointRule*>::iterator it = m_pPointRuleList.begin();
    for(;it != m_pPointRuleList.end();it++)
    {
        HPointRule* pointRule = (HPointRule*)*it;
        if(pointRule && ptRule != pointRule)
        {
            quint16 wOpenRuleFileID = pointRule->m_wOpenRuleFileID;
            quint16 wCloseRuleFileID = pointRule->m_wCloseRuleFileID;
            quint16 wJXOpenRuleFileID = pointRule->m_wJXOpenRuleFileID;
            quint16 wJXCloseRuleFileID = pointRule->m_wJXCloseRuleFileID;
            if(wOpenRuleFileID == ptRule->m_wOpenRuleFileID||wCloseRuleFileID == ptRule->m_wOpenRuleFileID||wJXOpenRuleFileID == ptRule->m_wOpenRuleFileID||wJXCloseRuleFileID == ptRule->m_wOpenRuleFileID)
                ptRule->m_wOpenRuleFileID = (quint16)-1;
            else if(wOpenRuleFileID == ptRule->m_wCloseRuleFileID||wCloseRuleFileID == ptRule->m_wCloseRuleFileID||wJXOpenRuleFileID == ptRule->m_wCloseRuleFileID||wJXCloseRuleFileID == ptRule->m_wCloseRuleFileID)
                ptRule->m_wCloseRuleFileID = (quint16)-1;
            else if(wOpenRuleFileID == ptRule->m_wJXOpenRuleFileID||wCloseRuleFileID == ptRule->m_wJXOpenRuleFileID||wJXOpenRuleFileID == ptRule->m_wJXOpenRuleFileID||wJXCloseRuleFileID == ptRule->m_wJXOpenRuleFileID)
                ptRule->m_wJXOpenRuleFileID = (quint16)-1;
            else if(wOpenRuleFileID == ptRule->m_wJXCloseRuleFileID||wCloseRuleFileID == ptRule->m_wJXCloseRuleFileID||wJXOpenRuleFileID == ptRule->m_wJXCloseRuleFileID||wJXCloseRuleFileID == ptRule->m_wJXCloseRuleFileID)
                ptRule->m_wJXCloseRuleFileID = (quint16)-1;
        }
    }
}
//////////////////////////////////////////////HStationRule////////////////////////////////
HStationRule::HStationRule()
{
    m_wStationNo = (qint16)-1;
    m_wRuleFileID = (quint16)-1;//规则文件的的ID
    m_pProtRuleList.clear();
}

HStationRule::~HStationRule()
{
    clear();
}

void HStationRule::clear()
{
    while(!m_pProtRuleList.isEmpty())
        delete (HProtectRule*)m_pProtRuleList.takeFirst();
    m_pProtRuleList.clear();
}

void HStationRule::maxRuleFileID()
{
    m_wRuleFileID = 0;
}

bool HStationRule::addProtectRule(HProtectRule* pProtRule)
{
    m_pProtRuleList.append(pProtRule);
    return true;
}

void HStationRule::readData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    quint16 n16;
    *ds >> n16;
    m_wStationNo = n16;
    *ds >> n16;
    m_wRuleFileID = n16;
    QString s;
    *ds >> s;
    m_strStationName = s;
    *ds >> s;
    m_strRuleFileName = s;

    *ds >> n16;
    quint16 wProtRuleCount = n16;
    for(int i = 0; i < wProtRuleCount;i++)
    {
        HProtectRule* protRule = new HProtectRule;
        protRule->readData(nVersion,ds);
    }
}

void HStationRule::writeData(int nVersion,QDataStream* ds)
{
    if(!ds)
        return;
    *ds<<(quint16)m_wStationNo;
    *ds<<(quint16)m_wRuleFileID;
    *ds<<(QString)m_strStationName;
    *ds<<(QString)m_strRuleFileName;
    quint16 wProtRuleCount = (quint16)m_pProtRuleList.count();
    *ds<<wProtRuleCount;
    for(int i = 0;i < wProtRuleCount;i++)
    {
        HProtectRule* protRule = (HProtectRule*)m_pProtRuleList[i];
        protRule->writeData(nVersion,ds);
    }
}

HProtectRule* HStationRule::protectRule(quint16 wProtNo)
{
    HProtectRule* pProtRule = NULL;
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        pProtRule = (HProtectRule*)m_pProtRuleList[i];
        if(pProtRule->m_wProtectNo == wProtNo)
            return pProtRule;
    }
    return NULL;
}

HRuleFile* HStationRule::ruleFile(quint16 wType,quint16 wPointNo,quint8 btYKType)
{
    HRuleFile* pRuleFile = NULL;
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)m_pProtRuleList[i];
        if(pProtRule)
        {
            pRuleFile = pProtRule->ruleFile(wType,wPointNo,btYKType);
            if(!pRuleFile)
                return pRuleFile;
        }
    }
    return NULL;
}

bool HStationRule::delRuleFile(quint16 wType,quint16 wPointNo,quint8 btYKType)
{
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)m_pProtRuleList[i];
        if(pProtRule)
        {
            if(pProtRule->delRuleFile(wType,wPointNo,btYKType))
                return true;
        }
    }
    return false;
}

bool HStationRule::delPointRule(quint16 wType,quint16 wPointNo)
{
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)m_pProtRuleList[i];
        if(pProtRule)
        {
            if(pProtRule->delPointRuleByID(wType,wPointNo))
                return true;
        }
    }
    return false;
}

HPointRule* HStationRule::getFirstPointRule()
{
    HPointRule* ptRule = NULL;
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        HProtectRule* proRule = (HProtectRule*)m_pProtRuleList[i];
        if(proRule)
        {
            ptRule = proRule->m_pPointRuleList.first();
        }
    }
    return ptRule;
}

bool HStationRule::changeStationNo(quint16 wNewStationNo)
{
    m_wStationNo = wNewStationNo;
    for(int i = 0; i < m_pProtRuleList.count();i++)
    {
        HProtectRule* pProtRule = (HProtectRule*)m_pProtRuleList[i];
        if(pProtRule)
        {
            pProtRule->m_wStationNo = m_wStationNo;
            //还要刷新装置下的测点和规则文件
            for(int i = 0; i < pProtRule->m_pRuleFileList.count();i++)
            {
                HRuleFile* rf = (HRuleFile*)pProtRule->m_pRuleFileList[i];
                if(rf)
                {
                    rf->m_ruleFileData.wStationNo = wNewStationNo;
                }
            }
            for(int i = 0; i < pProtRule->m_pPointRuleList.count();i++)
            {
                HPointRule* pr = (HPointRule*)pProtRule->m_pPointRuleList[i];
                if(pr)
                {
                    pr->m_wStationNo = wNewStationNo;
                }
            }

        }
    }

    //需要修正一下规则名称和规则名 在HRuledoc.cpp里面完成
    return false;
}

//////////////////////////////////////////////////HStationRuleList//////////////////////////////////////////////////////////////////////////////////
HStationRuleList::HStationRuleList()
{
    clear();
}

HStationRuleList::~HStationRuleList()
{
    clear();
}

void HStationRuleList::clear()
{
    while (!m_pStationRuleList.isEmpty()) {
        delete (HStationRule*)m_pStationRuleList.takeFirst();
    }
    m_pStationRuleList.clear();
}

void HStationRuleList::addStationRule(HStationRule* stRule)
{
    if(!stRule) return;
    m_pStationRuleList.append(stRule);
}

HStationRule* HStationRuleList::findStationRule(quint16 wStationID)
{
    QList<HStationRule*>::iterator it = m_pStationRuleList.begin();
    for(;it != m_pStationRuleList.end();++it)
    {
        HStationRule* pStRule = (HStationRule*)*it;
        if(pStRule && pStRule->m_wStationNo == wStationID)
            return pStRule;
    }
    return NULL;
}

void HStationRuleList::reloadStationRule()
{
    for(int i = 0; i < m_pStationRuleList.count();i++)
    {
        HStationRule* pStRule = (HStationRule*)m_pStationRuleList[i];
        if(pStRule)
        {
            HPointRule* ptRule = pStRule->getFirstPointRule();
            if(!ptRule) continue;
            RULEPARAM* ruleParam = new RULEPARAM;
            memset(ruleParam,0,sizeof(RULEPARAM));
            ruleParam->wStationNo = ptRule->m_wStationNo;
            ruleParam->wPointType = ptRule->m_wPointType;
            ruleParam->wPointNo = ptRule->m_wPointNo;

            if(m_lpRuleDataCallBack)
            {
                if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
                {
                    m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
                }
                else if(m_btAppType == TYPE_APP_LOCK)
                {
                    m_lpRuleDataCallBack(WM_GIN_GETDBINFO,ruleParam);
                }
            }

            pStRule->m_strStationName = ruleParam->strStationName;
            pStRule->m_strRuleFileName = QString("%1_%2.fma").arg(ruleParam->strStationName).arg(ruleParam->wStationNo);
        }
    }
}

///////////////////////////////////////////////////////////遍历函数部分////////////////////////////////////////////////////
//遍历主要是从结果往前推，而不是从前往后推
bool HRuleFile::buildGeneralFormula()
{
    bFormulaRight = true;
    m_strFormula = "";
    //遍历所有对象，是否存在未连接对象，同时设置遍历标记
    for(int i = 0; i < m_drawObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)m_drawObjList[i];
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
        for(int i = 0; i < m_drawObjList.count();i++)
        {
            HDrawObj* pObj = (HDrawObj*)m_drawObjList[i];
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
    if (m_strFormula.isEmpty())
		return false;

    return true;
}

bool HRuleFile::visitGeneralBuildObj(HDrawObj* pObj)
{
	int nOpCount;
	bool bErrorFlag = true;
    if (pObj->getObjType() == TYPE_INPUT)
	{
        m_strFormula = m_strFormula + pObj->getOperatorFirst();
        m_strFormula = m_strFormula + pObj->getOperatorMid();
        m_strFormula = m_strFormula + pObj->getOperatorLast();
	}
	else //(pObj->getObjType == TYPE_LOGICAND)
	{
        m_strFormula = m_strFormula + pObj->getOperatorFirst();
		for (nOpCount = 0; nOpCount < pObj->m_nInPointSum; nOpCount++)
		{
			HDrawObj* pConnObj = getConnectObj(pObj, nOpCount);
			if (pConnObj)
			{
				bErrorFlag = visitGeneralBuildObj(pConnObj);
				if (nOpCount < pObj->m_nInPointSum - 1)
                    m_strFormula = m_strFormula + pObj->getOperatorMid();
			}
			else
			{
				pObj->setWrongFlag(true);
                bFormulaRight = false;
			}
		}
        m_strFormula += pObj->getOperatorLast();
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
    quint32 nResultObjID = pResultObj->m_dwID;
    QList<HConnect*>::iterator it = m_connectObjList.begin();
    for(;it != m_connectObjList.end();++it)
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

/////////////////////////////////////////////////////////规则报告遍历///////////////////////////////////////////////////////////////////////////
bool HRuleFile::buildReportFormula()
{
    QList<QStringList*> reportList;

    //先找到最后一个输出
    HResultObj* pResultObj = getResultObj();
    if(!pResultObj) return false;

    //从输出对象开始找到第一个连接对象
    HDrawObj* pFirstObj = getConnectObj(pResultObj, 0);
    if(TYPE_INPUT == pFirstObj->getObjType())
    {
        QStringList* pStrList = new QStringList;
        pStrList->append(pFirstObj->m_strName);
        reportList.append(pStrList);
    }

    return visitReportBuildObj(pFirstObj,repor);
}

bool HRuleFile::visitReportBuildObj(HDrawObj* firstObj,QList<QStringList*> &reportList)
{
    if(TYPE_LOGICOR == firstObj->getObjType())
    {
        QList<QStringList*> orStrList;
        for(int orInNo = 0; orInNo < firstObj->m_nInPointSum;orInNo++)
        {
            HDrawObj* connObj = getConnectObj(firstObj,orInNo);
            if(!connObj) continue;
            if(connObj->getObjType() == TYPE_LOGICAND)
            {
                visitReportBuildObj(connObj,orStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_LOGICOR)
            {
                visitReportBuildObj(connObj,orStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_INPUT)
            {
                QStringList* pStrList = new QStringList;
                pStrList->append(connObj->m_strName);
                orStrList[orInNo]->append(pStrList);
            }
            else
                return false;
        }

        QList<QStringList*>::iterator it = orStrList.begin();
        for(; it < orStrList.end();it++)
        {
            QStringList* strList = (QStringList*)orStrList.takeFirst();
            reportList.append(strList);
            delete strList;
        }
    }
    else if(TYPE_LOGICAND == firstObj->getObjType())
    {
        /* 类似以下结构
         * A---
         *     |或 ----|
         * B---        |
         *             | 与 ---->
         * C---        |
         *     |或-----|
         * D---
         *
        */
        QList<QStringList*> andStrList;
        for(int andInNo = 0; andInNo < firstObj->m_nInPointSum;andInNo++)
        {
            HDrawObj* connObj = getConnectObj(firstObj,andInNo);
            if(!connObj) continue;
            if(connObj->getObjType() == TYPE_LOGICAND)
            {
                visitReportBuildObj(connObj,andStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_LOGICOR)
            {
                visitReportBuildObj(connObj,andStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_INPUT)
            {
                QStringList* pStrList = new QStringList;
                pStrList->append(connObj->m_strName);
                andStrList[andInNo]->append(pStrList);
            }
            else
                return false;
        }

        //and前面如果有or的话 or的每项都要和其他的交互
        for(int i = 0;i < andStrList.count();i++)
        {
            QStringList* pStrList = andStrList[i]; //A
            reportList[i]->append(pStrList);
            for(int j = i+1; j < andStrList.count();i++)
        }

    }
}

void getOrItem()
{
    QList<QStringList*> andStrList;
    QList<QStringList*> reportList;
    for(int i = 0;i < andStrList.count();i++)
    {
        QStringList* pStrList = andStrList[i]; //A
        reportList[i]->append(pStrList);

    }
}
