#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hruledoc.h"
#include <QDir>
#include <QProcessEnvironment>
#include "hrulefile.h"
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;

HRuleDoc::HRuleDoc()
{
    m_bModify = false;
    m_pStationRuleList = new HStationRuleList;
    m_pRuleFile = NULL;
    m_pTempRuleFile = NULL;
}

HRuleDoc::~HRuleDoc()
{
    if(m_pStationRuleList){
        delete m_pStationRuleList->takeFirst();
        m_pStationRuleList = NULL;
    }
    m_pRuleFile = NULL;
    m_pTempRuleFile = NULL;
}

//加载所有规则
bool HRuleDoc::loadRuleFiles()
{
    //QString strRuleFile = g_strRuleFilePath + "*.RUF";
    //xxx/rule/station_0.fma
    QString wfPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    wfPath.append("/rule");
    if(m_pStationRuleList)
        m_pStationRuleList->clear();
    QDir dirRulePath(wfPath);
    if(!dirRulePath.exists())
        return false;
    QStringList filters;
    filters<<"*.RUF";//RUL RLF
    dirRulePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirRulePath.entryInfoList(QDir::Files  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(!info.isFile())continue;
        QString strRuleFile = wfPath + "/" + info.fileName();
        QFile file(strRuleFile);
        if(!file.exists() || !file.open(QIODevice::ReadOnly))
            continue;
        QDataStream stream(&file);
        HStationRule* pStationRule = new HStationRule;
        if(pStationRule)
            pStationRule->readData(QDataStream::Qt_5_7,&stream);

        //要进行厂站ID的判断
        if(m_pStationRuleList->findStationRule(pStationRule->m_wStationNo))
        {
            QString strErrMsg = QString("厂站地址=%1的规则已经存在，导入失败！").arg(pStationRule->m_wStationNo);
            //弹出对话框
            delete pStationRule;
            pStationRule = NULL;
            return false;
        }
        m_pStationRuleList->addStationRule(pStationRule);
    }
    return true;
}

bool HRuleDoc::saveRuleFiles()
{
    //d:/wfPath/rule = g_strRuleFilePath
    //保存之前厂站要刷一下地址和站名，发生发生变化导致错误
    m_pStationRuleList->reloadStationRule();
    QString wfPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    wfPath.append("/rule");
    QDir wfDir(wfPath);
    if(!wfDir.exists())
        wfDir.mkdir(wfPath);
    for(int i = 0; i < m_pStationRuleList->count();i++)
    {
        HStationRule* pStRule = (HStationRule*)m_pStationRuleList->at(i);
        if(pStRule)
        {
            //qt下面必须要先删除，然后才能重新写入
            delRuleProFile(pStRule->m_wStationNo);
            QByteArray bytes;
            QDataStream stream(&bytes,QIODevice::WriteOnly);
            pStRule->writeData(QDataStream::Qt_5_7,&stream);
            QString strRuleFile = wfPath + "/" + pStRule->m_strRuleFileName + ".RUF";
            QFile file(strRuleFile);
            if(file.open(QIODevice::WriteOnly))
            {
                QDataStream cbStream(&file);
                cbStream.writeRawData(bytes.data(),bytes.length());
                file.close();
            }
        }
    }
    return true;
}

bool HRuleDoc::delRuleProFile(quint16 wStationNo)
{
    //先删除之前的规则文件
    QString wfPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    wfPath.append("/rule");
    //if(m_pStationRuleList)
    //    m_pStationRuleList->clear();
    QDir dirRulePath(wfPath);
    if(!dirRulePath.exists())
        return false;
    HStationRule* pStRule = m_pStationRuleList->findStationRule(wStationNo);
    if(!pStRule)
        return false;
    QStringList filters;
    filters<<"*.RUF";
    dirRulePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirRulePath.entryInfoList(QDir::Files  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(!info.isFile())continue;
        QString fileName = info.fileName();
        if(pStRule->m_strRuleFileName.compare(fileName))
        {
            QString strProFilePath = wfPath + "/" + fileName;
            return QFile::remove(strProFilePath);
        }
    }
    m_bModify = true;
    return true;
}

/*
 * 函数说明:根据参数获取对应的规则文件
 * 备注:获取存储文件里面的规则文件，应该拷贝给临时文件，后续所有操作都在临时文件内。
 * 确定保存后，将临时文件再次拷贝到实际规则文件，确定保存。如果不保存，则丢弃临时规则文件，并不影响实际规则文件
*/
HRuleFile* HRuleDoc::getRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType,quint16 wRuleID, quint8 btRuleType, QString &strFormula)
{

    if(m_pTempRuleFile)
    {
        delete m_pTempRuleFile;
        m_pTempRuleFile = NULL;
    }

   /* if(m_pRuleFile)
    {
        delete m_pRuleFile;
        m_pRuleFile = NULL;
    }*/

    quint16 wStID;//站号
    quint16 wProtID; //装置ID 联锁组态用
    quint16 wPtTypeID; //测点类型 五防用
    quint16 wPtNo;//当联锁组态时为GIN号，当是测点类型时为点号

    quint8 btInsideType;
    quint16 wOpenFormulaID;
    quint16 wCloseFormulaID;
    quint16 wOpenJXFormulaID;
    quint16 wCloseJXFormulaID;

    QString strStationName;
    QString strProtectName;//装置/间隔
    QString strPointName;

    RULEINFO* ruleParam = new RULEINFO;
    ruleParam->wStationNo = (quint16)-1;
    ruleParam->wDeviceNo = (quint16)-1;
    ruleParam->btPointType = (quint8)-1;
    ruleParam->wPointNo = (quint16)-1;
    ruleParam->wAttr = 0;

    ruleParam->btInsideType = (quint8)-1;
    ruleParam->wOpenFormulaID = (quint16)-1;
    ruleParam->wCloseFormulaID = (quint16)-1;
    ruleParam->wOpenJXFormulaID = (quint16)-1;
    ruleParam->wCloseJXFormulaID = (quint16)-1;

    ruleParam->wStationNo = wStationNo;
    ruleParam->wPointNo = wPointNo;

    if(m_lpRuleDataCallBack)
    {
        if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
        {
            ruleParam->btPointType = wPointType;
            m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
        }
        else if(m_btAppType == TYPE_APP_LOCK)
        {
            ruleParam->wDeviceNo = wPointType;
            m_lpRuleDataCallBack(WM_GIN_GETDBINFO,ruleParam);
        }
    }
    else
        return NULL;

    wOpenFormulaID = ruleParam->wOpenFormulaID;
    wCloseFormulaID = ruleParam->wCloseFormulaID;
    wOpenJXFormulaID = ruleParam->wOpenJXFormulaID;
    wCloseJXFormulaID = ruleParam->wCloseJXFormulaID;
    btInsideType = ruleParam->btInsideType;
    strStationName = ruleParam->strStationName;
    strPointName = ruleParam->strPointName;
    strProtectName = ruleParam->strProtectName;

    if(m_btAppType == TYPE_APP_JK)
    {
        wStID = ruleParam->wStationNo;
        wProtID = ruleParam->wDeviceNo;
        wPtNo = ruleParam->wPointNo;
        wPtTypeID = ruleParam->btPointType;
    }
    else if(m_btAppType = TYPE_APP_WF)
    {
        wStID = ruleParam->wStationNo;
        wProtID = 0;
        wPtNo = ruleParam->wPointNo;
        wPtTypeID = ruleParam->btPointType;
    }
    //还有其他的----huangw


    //对应厂站/间隔/测点来获取具体的规则信息
    HStationRule* stationRule = (HStationRule*)m_pStationRuleList->findStationRule(wStID);
    if(!stationRule)
    {
        stationRule = new HStationRule;
        stationRule->m_wStationNo = wStID;
        stationRule->m_strStationName = strStationName;
        QString strRuleName = QString("%1_%2.RUF").arg(strStationName).arg(wStationNo);
        stationRule->m_strRuleFileName = strRuleName;
        m_pStationRuleList->addStationRule(stationRule);
    }

    HDeviceRule* protectRule = stationRule->protectRule(wProtID);//间隔，装置
    if(!protectRule)
    {
        protectRule = new HDeviceRule;
        protectRule->m_wStationNo = wStID;
        protectRule->m_wDeviceNo = wProtID;
        protectRule->m_strDeviceName = strProtectName;
        stationRule->addProtectRule(protectRule);
    }

    HPointRule* pointRule = protectRule->pointRule(wPtTypeID,wPtNo);
    if(!pointRule)
    {
        pointRule = new HPointRule;
        pointRule->m_wStationNo = wStID;
        pointRule->m_wDeviceNo = wProtID;
        pointRule->m_wPointNo = wPtNo;
        pointRule->m_wPointType = wPointType;
        pointRule->m_strPointName = strPointName;
        protectRule->addPointRule(pointRule);
    }

    //规则ID
    pointRule->m_btInsideType = btInsideType;
    pointRule->m_wOpenFormulaID = wOpenFormulaID;
    pointRule->m_wCloseFormulaID = wCloseFormulaID;
    pointRule->m_wJXOpenFormulaID = wOpenJXFormulaID;
    pointRule->m_wJXCloseRuleFileID = wCloseJXFormulaID;

    /*
     * 删除某个测点规则时，只删除规则文件，并未删除测点。所以此处需要调整
     * 如果删除规则文件同时删除测点，此处不需要调整
    */
    protectRule->refreshRuleFileID(pointRule);


    //规则文件ID
    quint16 wRuleFileID;
    if(CTRL_OPEN == btYKType)
        wRuleFileID = pointRule->m_wOpenRuleFileID;
    else if(CTRL_CLOSE == btYKType)
        wRuleFileID = pointRule->m_wCloseRuleFileID;
    else if(CTRL_JXOPEN == btYKType)
        wRuleFileID = pointRule->m_wJXOpenRuleFileID;
    else if(CTRL_JXCLOSE == btYKType)
        wRuleFileID = pointRule->m_wJXCloseRuleFileID;

    m_pRuleFile = (HRuleFile*)protectRule->getRuleFileByID(wRuleFileID);
    if(!m_pRuleFile)
    {
        m_pRuleFile = new HRuleFile;
        m_pRuleFile->m_wRuleFileID = stationRule->generateID();
        protectRule->addRuleFile(m_pRuleFile);
    }

    //复制给规则文件
    m_pRuleFile->m_ruleFileData.wStationNo = wStID;
    m_pRuleFile->m_ruleFileData.wDeviceNo = wProtID;
    m_pRuleFile->m_ruleFileData.wPointNo = wPtNo;
    m_pRuleFile->m_ruleFileData.btPointType = wPointType;//测点类型 遥测 遥信 遥控
    m_pRuleFile->m_ruleFileData.btYKType = btYKType;
    m_pRuleFile->m_ruleFileData.btFormulaType = btRuleType;//规则类型:普通规则、二级规则
    //pRuleFile->m_ruleFileData.wFormulaID = ;//测点分/合等规则ID号

    QString strYKRule;//遥控规则类型：分、合、检修分、检修合
    if(CTRL_OPEN == btYKType)
    {
        strYKRule = "分规则";
        m_pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wOpenFormulaID;
        pointRule->m_wOpenRuleFileID = m_pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_CLOSE == btYKType)
    {
        strYKRule = "合规则";
        m_pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wCloseFormulaID;
        pointRule->m_wCloseRuleFileID = m_pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_JXOPEN == btYKType)
    {
        strYKRule = "检修分规则";
        m_pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXOpenFormulaID;
        pointRule->m_wJXOpenRuleFileID = m_pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_JXCLOSE == btYKType)
    {
        strYKRule = "检修合规则";
        m_pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXCloseFormulaID;
        pointRule->m_wJXCloseRuleFileID = m_pRuleFile->m_wRuleFileID;
    }

    m_pRuleFile->m_strRuleName = QString("%1%2%3").arg(strStationName).arg(strPointName).arg(strYKRule);

    //读取完还要刷新一下所有测点规则，防止站名，站地址发生改变后无法显示正确规则
    m_pRuleFile->refreshDrawObj();

    //拷贝给临时规则文件
    m_pTempRuleFile = m_pRuleFile->clone();

    delete ruleParam;
    return m_pTempRuleFile;
    //获取规则文件后,后续就是应该在界面上显示规则文件所有图符
}

void HRuleDoc::exportAllRule(quint16 wStationNo)
{

}

bool HRuleDoc::isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    HStationRule* pStRule = m_pStationRuleList->findStationRule(wStationNo);
    if(!pStRule)
        return false;
    HRuleFile* rf = (HRuleFile*)pStRule->ruleFile(wPointType,wPointNo,btYKType);
    if(!rf)
        return false;
    return true;
}

bool HRuleDoc::delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btYKType)
{
    HStationRule* pStRule = m_pStationRuleList->findStationRule(wStationNo);
    if(!pStRule)
        return false;
    return pStRule->delRuleFile(wPointType,wPointNo,btYKType);
}

void HRuleDoc::changeStationID(quint16 wStNo,quint16 wNewStNo)
{
    HStationRule* pStRule = m_pStationRuleList->findStationRule(wStNo);
    if(!pStRule)
        return;

    //修改ID
    pStRule->changeStationNo(wNewStNo);
}
