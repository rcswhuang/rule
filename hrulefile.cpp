#include "hrulefile.h"
#include "hruleeditapi.h"
#include <QList>
#include <QProcessEnvironment>
#include <QDir>
#include <QCoreApplication>
#include <QElapsedTimer>
#ifdef Q_OS_WIN
 #include <Windows.h>
#else
 #include<time.h>
#endif

extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;
/*
 * 定义一个sleep的全局函数
*/
void qSleep(int ms)
{
#ifdef Q_OS_WIN
    Sleep(uint(ms));
#else
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#endif
}


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

HRuleFile* HRuleFile::clone()
{
    QString clonePath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    clonePath.append("/temp/rule");
    QDir dir(clonePath);
    if(!dir.exists())
        dir.mkdir(clonePath);
    clonePath.append("/clone.tmp");

    QByteArray bytes;
    QDataStream stream(&bytes,QIODevice::WriteOnly);
    writeData(QDataStream::Qt_5_7,&stream);
    QFile file(clonePath);
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream cbStream(&file);
        cbStream.writeBytes(bytes.data(),bytes.length());
        file.close();
    }

    qSleep(5000);

    HRuleFile* ruleFile = new HRuleFile;
    QFile file1(clonePath);
    if(!file1.exists() || !file1.open(QIODevice::ReadOnly))
        return NULL;
    QDataStream stream1(&file1);
    ruleFile->readData(QDataStream::Qt_5_7,&stream1);
    file1.close();
    QFile::remove(clonePath);
    return ruleFile;
}

void HRuleFile::copyTo(HRuleFile *rf)
{
    if(!rf) return;
    //rf读
    rf->clear();
    QString copyPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    copyPath.append("/temp/rule");
    QDir dir(copyPath);
    if(!dir.exists())
        dir.mkdir(copyPath);
    copyPath.append("/copy.tmp");

    QByteArray bytes;
    QDataStream stream(&bytes,QIODevice::WriteOnly);
    writeData(QDataStream::Qt_5_7,&stream);
    QFile file(copyPath);
    if(file.open(QIODevice::WriteOnly))
    {
        QDataStream cbStream(&file);
        cbStream.writeBytes(bytes.data(),bytes.length());
        file.close();
    }

    qSleep(5000);

    QFile file1(copyPath);
    if(!file1.exists() || !file1.open(QIODevice::ReadOnly))
        return;
    QDataStream stream1(&file1);
    rf->readData(QDataStream::Qt_5_7,&stream1);
    file1.close();
    QFile::remove(copyPath);

    memcpy(&rf->m_ruleFileData,&m_ruleFileData,sizeof(RULEFILEDATA));

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
        HConnect* conn = new HConnect;
        if(conn)
        {
            conn->readData(nVersion,ds);
            conn->m_pRuleFile = this;
            m_connectObjList.append(conn);
        }
    }
    int nDrawObjCount;
    *ds >> n;
    nDrawObjCount = n;
    for(int i = 0; i < nDrawObjCount;i++)
    {
        quint8 btObjType;
        *ds>>btObjType;
        HDrawObj* pObj = NULL;
        QPoint point = QPoint(0,0);
        if(TYPE_INPUT == btObjType)
        {
            pObj = new HInputObj(QRect(point,QSize(100,40)),this);
        }
        else if(TYPE_LOGICAND == btObjType)
        {
            pObj = new HAndObj(QRect(point,QSize(80,84)),this);
        }
        else if(TYPE_LOGICOR == btObjType)
        {
            pObj = new HOrObj(QRect(point,QSize(80,84)),this);
        }
        else if(TYPE_RESULT == btObjType)
        {
            pObj = new HResultObj(QRect(point,QSize(100,40)),this);
        }
        else
            return;
        pObj->readData(nVersion,ds);
        pObj->m_pRuleFile = this;
        m_drawObjList.append(pObj);
     }

    //需要重新刷新一个ID
    refreshDrawObjID();

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
        {
            *ds<<drawObj->getObjType();
            drawObj->writeData(nVersion,ds);
        }
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
    //QRect rect(point, QSize(2, 2));
    for (int i = 0; i < m_connectObjList.count(); i++)
	{
        HConnect* pObj = (HConnect*)m_connectObjList[i];
        if (pObj->intersects(point))
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
            if (pConnect->m_dwInObjID == dwObjID || pConnect->m_dwOutObjID == dwObjID)
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
        if (conn && conn->m_dwOutObjID == dwTargetObjID && conn->m_btOutIndex == nConnNo)
		{
            dwConnectedID = conn->m_dwInObjID;
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

void HRuleFile::refreshDrawObj()
{
    for(int i = 0; i < m_drawObjList.count();i++)
    {
        HDrawObj* drawObj = (HDrawObj*)m_drawObjList[i];
        if(drawObj && TYPE_INPUT == drawObj->getObjType())
        {
            //可以作为函数剥离出来，此处放在一处
            if(TYPE_APP_JK == m_btAppType || TYPE_APP_WF == m_btAppType)
            {
                HInputObj* inputDrawObj = (HInputObj*)drawObj;
                if(TYPE_INPUT_DIGITAL == inputDrawObj->btInputType)
                {
                    QString strStName,strProtName,strPtName,strAttr,strCond,strFormula,strContent;
                    RULEPARAM *ruleParam = new RULEPARAM;
                    memset(ruleParam,0,sizeof(RULEPARAM));
                    ruleParam->wStationNo = inputDrawObj->m_wStationID1;
                    ruleParam->wPointNo = inputDrawObj->m_wPointID1;
                    ruleParam->btPointType = inputDrawObj->m_btType1;

                    if(m_lpRuleDataCallBack)
                    {
                        m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
                        strStName = ruleParam->strStationName;
                        strProtName = ruleParam->strProtectName;
                        strPtName = ruleParam->strPointName;
                        strAttr = ruleParam->strAttr;
                    }
                    else
                    {
                        strStName = "";
                        strProtName = "";
                        strPtName = "";
                        strAttr = "";
                    }
                    if(COND_CLOSE ==inputDrawObj->m_btCondition)
                    {
                        strCond = "合位置";
                        strFormula = "["+strStName+"."+strPtName+"."+strAttr+"]";
                        strContent = strPtName + "=" +strCond;
                    }
                    else
                    {
                        strCond = "分位置";
                        strFormula = "~["+strStName+"."+strPtName+"."+strAttr+"]";
                        strContent = strPtName + "=" +strCond;
                    }
                    inputDrawObj->m_strName = strContent;
                    inputDrawObj->m_strRuleName = strFormula;
                    if(ruleParam)
                    {
                        delete ruleParam;
                        ruleParam = NULL;
                    }
                }
                else if(TYPE_INPUT_COMP == inputDrawObj->btInputType)
                {
                    QString strStName1,strProtName1,strPtName1,strAttr1;
                    QString strStName2,strProtName2,strPtName2,strAttr2;
                    QString strFormula,strContent;
                    RULEPARAM *ruleParam = new RULEPARAM;

                    //测点1
                    memset(ruleParam,0,sizeof(RULEPARAM));
                    ruleParam->wStationNo = inputDrawObj->m_wStationID1;
                    ruleParam->wPointNo = inputDrawObj->m_wPointID1;
                    ruleParam->btPointType = inputDrawObj->m_btType1;

                    if(m_lpRuleDataCallBack)
                    {
                        m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
                        strStName1 = ruleParam->strStationName;
                        strProtName1 = ruleParam->strProtectName;
                        strPtName1 = ruleParam->strPointName;
                        strAttr1 = ruleParam->strAttr;
                    }
                    else
                    {
                        strStName1 = "";
                        strProtName1 = "";
                        strPtName1 = "";
                        strAttr1 = "";
                    }

                    //测点2
                    memset(ruleParam,0,sizeof(RULEPARAM));
                    ruleParam->wStationNo = inputDrawObj->m_wStationID2;
                    ruleParam->wPointNo = inputDrawObj->m_wPointID2;
                    ruleParam->btPointType = inputDrawObj->m_btType2;

                    if(m_lpRuleDataCallBack)
                    {
                        m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
                        strStName2 = ruleParam->strStationName;
                        strProtName2 = ruleParam->strProtectName;
                        strPtName2 = ruleParam->strPointName;
                        strAttr2 = ruleParam->strAttr;
                    }
                    else
                    {
                        strStName2 = "";
                        strProtName2 = "";
                        strPtName2 = "";
                        strAttr2 = "";
                    }

                    QString strComp1;
                    QString strComp2;
                    quint8 nComp2 = inputDrawObj->m_btCompType;

                    strComp1 = "["+strStName1+"."+strPtName1+"."+ strAttr1+"]";
                    if(TYPE_COMPARE_CONST == nComp2)
                    {
                        strComp2 = QString("%1").arg(inputDrawObj->m_fCompValue);
                    }
                    else
                    {
                        strComp2 = "["+strStName2+"."+strPtName2+"."+ strAttr2+"]";
                    }

                    QString strCond;//条件表达式
                    quint8 nCond = inputDrawObj->m_btCondition;
                    switch(nCond)
                    {
                    case OP_GREATER:  //>
                        strCond = ">";
                        break;
                    case OP_LOWER:  // <
                        strCond = "<";
                        break;
                    case OP_EQUAL: // =
                        strCond = "=";
                        break;
                    case OP_GEQUAL: //>=
                        strCond = ">=";
                        break;
                    case OP_LEQUAL: //<=
                        strCond = "<=";
                        break;
                    case OP_NEQUAL:
                        strCond = "!=";
                        break;
                    }
                    strFormula = "("+strComp1+strCond+strComp2+")";
                    strContent = strComp1 + strCond + strComp2;
                    inputDrawObj->m_strName = strContent;
                    inputDrawObj->m_strRuleName = strFormula;
                    if(ruleParam)
                    {
                        delete ruleParam;
                        ruleParam = NULL;
                    }
                }
            }
        }
    }
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

    while(!m_pRuleFileList.isEmpty())
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
    return false;
}

QString HStationRule::getRuleFileText()
{
    QString strStRuleTxt;
    QString strPointTxt;
    QString strOpenRule,strCloseRule,strJXOpenRule,strJXCloseRule;
    strStRuleTxt += "************************************************** \n";
    strStRuleTxt += m_strStationName;
    strStRuleTxt += "\n";
    for(int i = 0;i < m_pProtRuleList.count();i++)
    {
        HProtectRule* protRule = (HProtectRule*)m_pProtRuleList[i];
        if(!protRule) continue;
        for(int k = 0; k < protRule->m_pPointRuleList.count();k++)
        {
            HPointRule* ptRule = (HPointRule*)protRule->m_pPointRuleList[i];
            HRuleFile* pOpenRule = (HRuleFile*)protRule->getRuleFileByID(ptRule->m_wOpenRuleFileID);
            if(pOpenRule)
            {
                pOpenRule->getRuleReport(strOpenRule);
            }
            else
                strOpenRule = "";

            HRuleFile* pCloseRule = (HRuleFile*)protRule->getRuleFileByID(ptRule->m_wCloseRuleFileID);
            if(pCloseRule)
            {
                pCloseRule->getRuleReport(strCloseRule);
            }
            else
                strCloseRule = "";

            HRuleFile* pJXOpenRule = (HRuleFile*)protRule->getRuleFileByID(ptRule->m_wJXOpenRuleFileID);
            if(pJXOpenRule)
            {
                pJXOpenRule->getRuleReport(strJXOpenRule);
            }
            else
                strJXOpenRule = "";

            HRuleFile* pJXCloseRule = (HRuleFile*)protRule->getRuleFileByID(ptRule->m_wJXCloseRuleFileID);
            if(pJXCloseRule)
            {
                pJXCloseRule->getRuleReport(strJXCloseRule);
            }
            else
                strJXCloseRule = "";

            strPointTxt += "======================================= \n";
            strPointTxt += ptRule->m_strPointName;
            strPointTxt += "\n ----------------------------------------\n";
            strPointTxt += "分规则: \n";
            if(strOpenRule.isEmpty())
                strPointTxt += "\n";
            else
                strPointTxt += splitRuleText(strOpenRule);
            strPointTxt += "--------------------------------------- \n";

            strPointTxt += "合规则: \n";
            if(strOpenRule.isEmpty())
                strPointTxt += "\n";
            else
                strPointTxt += splitRuleText(strCloseRule);
            strPointTxt += "--------------------------------------- \n";

            strPointTxt += "备用分规则: \n";
            if(strOpenRule.isEmpty())
                strPointTxt += "\n";
            else
                strPointTxt += splitRuleText(strJXOpenRule);
            strPointTxt += "--------------------------------------- \n";

            strPointTxt += "备用合规则: \n";
            if(strOpenRule.isEmpty())
                strPointTxt += "\n";
            else
                strPointTxt += splitRuleText(strJXCloseRule);
            strPointTxt += "--------------------------------------- \n";

            strStRuleTxt += strPointTxt;

        }
    }
    strStRuleTxt += "************************************************** \n";
    return strStRuleTxt;
}

QString HStationRule::splitRuleText(QString& strRuleText)
{
    QString strRuleItem="";
    QStringList strRuleList = strRuleText.split('@');
    for(int i = 0; i < strRuleList.count();i++)
    {
        QString strRule = strRuleList.at(i);
        QStringList strGroupRuleList = strRule.split('#');
        for(int j = 0; j < strGroupRuleList.count();j++)
        {
            strRuleItem += strGroupRuleList.at(i);
            strRuleItem = "\n";
        }
    }
    return strRuleItem;
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
            ruleParam->btPointType = ptRule->m_wPointType;
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
//遍历:从后到前
bool HRuleFile::buildGeneralFormula()
{
    m_bFormulaRight = true;
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
            m_bFormulaRight = false;
        }
    }
	else
	{
        pResultObj->setWrongFlag(false);
        m_bFormulaRight = false;
	}

    //检查所有元素是否访问
    for(int i = 0; i < m_drawObjList.count();i++)
    {
        HDrawObj* pObj = (HDrawObj*)m_drawObjList[i];
        if(pObj && !pObj->m_bVisit)
        {
            pObj->setWrongFlag(true);
            m_bFormulaRight = false;
        }
    }

    if (!m_bFormulaRight)
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
                bErrorFlag = false;
				pObj->setWrongFlag(true);
                m_bFormulaRight = false;
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
       if(conn->m_dwOutObjID == nResultObjID)
       {
           nConnObjID = conn->m_dwInObjID;
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

void HRuleFile::getRuleReport(QString& strRuleReport)
{
    QList<QStringList*> reportList;
    buildReportFormula(&reportList);

    /*
     * 每组之间用@分隔符，组内每条规则用#分割，便于后面解析
    */
    QString strRuleText;
    for(int i = 0;i < reportList.count();i++)
    {
        QStringList* plist = (QStringList*)reportList.at(i);
        if(plist)
        {
            strRuleText = QString("条件%1").arg(i+1);
            if(i > 0)
                strRuleText = QString("或条件%1").arg(i+1);
            //strRuleReport
            strRuleText += "#";
            for(int t = 0; t < plist->count();t++)
            {
                QString strRule = QString("%1.").arg(t+1) + plist->at(t); //1.xxxx = 合位置
                strRuleText += strRule;
                if(t < plist->count() - 1) //除最后一个外，其他都要加#分割
                    strRuleText += "#";
            }
            strRuleReport += strRuleText;
        }
        strRuleReport+="@";
    }
}

bool HRuleFile::buildReportFormula(QList<QStringList*> *reportList)
{
    //QList<QStringList*> reportList;
    //先找到最后一个输出
    HResultObj* pResultObj = getResultObj();
    if(!pResultObj) return false;

    //从输出对象开始找到第一个连接对象
    HDrawObj* pFirstObj = getConnectObj(pResultObj, 0);
    if(TYPE_INPUT == pFirstObj->getObjType())
    {
        QStringList* pStrList = new QStringList;
        pStrList->append(pFirstObj->m_strName);
        reportList->append(pStrList);
    }

    return visitReportBuildObj(pFirstObj,reportList);
}

bool HRuleFile::visitReportBuildObj(HDrawObj* firstObj,QList<QStringList*> *reportList)
{
    if(TYPE_LOGICOR == firstObj->getObjType())
    {
        QList<QStringList*> orStrList[MAXCOUNT_INPUT];
        for(int orInNo = 0; orInNo < firstObj->m_nInPointSum;orInNo++)
        {
            HDrawObj* connObj = getConnectObj(firstObj,orInNo);
            if(!connObj) continue;
            if(connObj->getObjType() == TYPE_LOGICAND)
            {
                visitReportBuildObj(connObj,&orStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_LOGICOR)
            {
                visitReportBuildObj(connObj,&orStrList[orInNo]);
            }
            else if(connObj->getObjType() == TYPE_INPUT)
            {
                QStringList* pStrList = new QStringList;
                pStrList->append(connObj->m_strName);
                orStrList[orInNo].append(pStrList);
            }
            else
                return false;
        }

        for(int i = 0; i < MAXCOUNT_INPUT;i++)
        {
            QList<QStringList*>::iterator it = orStrList[i].begin();
            for(; it < orStrList[i].end();it++)
            {
                QStringList* strList = (QStringList*)*it;
                reportList->append(strList);
            }
        }

        for(int i = 0; i < MAXCOUNT_INPUT;i++)
        {
            while(!orStrList[i].isEmpty())
                delete (QStringList*)orStrList[i].takeFirst();
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
        //初步设置为MAXCOUNT_INPUT,也可以用firstObj->m_nInPointSum来代替
        QList<QStringList*> andStrList[MAXCOUNT_INPUT];
        for(int andInNo = 0; andInNo < firstObj->m_nInPointSum;andInNo++)
        {
            HDrawObj* connObj = getConnectObj(firstObj,andInNo);
            if(!connObj) continue;
            if(connObj->getObjType() == TYPE_LOGICAND)
            {
                visitReportBuildObj(connObj,&andStrList[andInNo]);
            }
            else if(connObj->getObjType() == TYPE_LOGICOR)
            {
                visitReportBuildObj(connObj,&andStrList[andInNo]);
            }
            else if(connObj->getObjType() == TYPE_INPUT)
            {
                QStringList* pStrList = new QStringList;
                pStrList->append(connObj->m_strName);
                andStrList[andInNo].append(pStrList);
            }
            else
                return false;
        }

        //and前面如果有or的话 or的每项都要和其他的交互
        QStringList mStrList;
        getOrBeforeAnd(andStrList,reportList,&mStrList,0,firstObj->m_nInPointSum);

        for(int i = 0; i < MAXCOUNT_INPUT;i++)
        {
            while(!andStrList[i].isEmpty())
                delete (QStringList*)andStrList[i].takeFirst();
        }
    }
    return true;
}

void HRuleFile::getOrBeforeAnd(QList<QStringList*> *pSrcList,QList<QStringList*> *pDscList,QStringList* pTempList,int i,int total)
{
    int order = i;
    for(int p = 0; p < pSrcList[order].count();p++)
    {
        QStringList* pList = pSrcList[order].at(p);
        if(pList && order < total)
        {
            QStringList strList;
            for(int t = 0; t < pTempList->count();t++)
            {
                strList.append(pTempList->at(t));
            }

            for(int t = 0; t< pList->count();t++)
            {
                QString str = pList->at(t);
                strList.append(str);
            }

            if(order == total - 1)
            {
                QStringList* newList = new QStringList;
                for(int t =0; t < strList.count();t++)
                {
                    newList->append(strList[t]);
                }
                pDscList->append(newList);
                strList.clear();
            }

            i = order+1;
            if(i < total)
            {
                getOrBeforeAnd(pSrcList,pDscList,&strList,i,total);
            }
        }
    }
}
