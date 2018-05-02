#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hruledoc.h"
#include <QDir>
#include <QProcessEnvironment>
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;

HRuleDoc::HRuleDoc()
{
    m_bModify = false;
}

HRuleDoc::~HRuleDoc()
{

}

//加载所有规则
bool HRuleDoc::loadRuleFiles()
{
    //QString strRuleFile = g_strRuleFilePath + "*.fma";
    //xxx/rule/station_0.PRO
    QDir dirRulePath(m_strRuleFilePath);
    if(!dirRulePath.exists())
        return false;
    QStringList filters;
    filters<<"*.fma";
    dirRulePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirRulePath.entryInfoList(QDir::Files  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(!info.isFile())continue;
        QString strRuleFile = m_strRuleFilePath + "/" + info.fileName();
        QFile file(strRuleFile);
        if(!file.exists() || !file.open(QIODevice::ReadOnly))
            continue;
        QDataStream stream(&file);
        HStationRule* pStationRule = new HStationRule;
        if(pStationRule)
            pStationRule->readData(QDataStream::Qt_5_7,&stream);

        //要进行厂站ID的判断
        if(m_StationRuleList.findStationRule(pStationRule->m_wStationNo))
        {
            QString strErrMsg = QString("厂站地址=%1的规则已经存在，导入失败！").arg(pStationRule->m_wStationNo);
            //弹出对话框
            delete pStationRule;
            pStationRule = NULL;
            return false;
        }
        m_StationRuleList.addStationRule(pStationRule);
    }

    return true;
}

void HRuleDoc::saveRuleFiles()
{
    //d:/wfPath/rule = g_strRuleFilePath
    //保存之前厂站要刷一下地址和站名，发生发生变化导致错误
    m_StationRuleList.reloadStationRule();
    QString wfPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    wfPath.append("/rule");
    QDir wfDir(wfPath);
    if(!wfDir.exists())
        wfDir.mkdir(wfPath);
    for(int i = 0; i < m_StationRuleList.count();i++)
    {
        HStationRule* pStRule = (HStationRule*)m_StationRuleList.at(i);
        if(pStRule)
        {
            //qt下面必须要先删除，然后才能重新写入
            delRuleProFile(pStRule->m_wStationNo);
            QByteArray bytes;
            QDataStream stream(&bytes,QIODevice::WriteOnly);
            pStRule->writeData(QDataStream::Qt_5_7,&stream);
            QString strRuleFile = wfPath + "/" + pStRule->m_strRuleFileName;
            QFile file(strRuleFile);
            if(file.open(QIODevice::WriteOnly))
            {
                QDataStream cbStream(&file);
                cbStream.writeBytes(bytes.data(),bytes.length());
                file.close();
            }
        }
    }
}

bool HRuleDoc::delRuleProFile(quint16 wStationNo)
{
    //先删除之前的规则文件
    HStationRule* pStRule = m_StationRuleList.findStationRule(wStationNo);
    if(!pStRule)
        return false;
    QDir dirRulePath(m_strRuleFilePath);
    if(!dirRulePath.exists())
        return false;
    QStringList filters;
    filters<<"*.fma";
    dirRulePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirRulePath.entryInfoList(QDir::Files  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(!info.isFile())continue;
        QString fileName = info.fileName();
        if(pStRule->m_strRuleFileName.compare(fileName))
        {
            QString strProFilePath = m_strRuleFilePath + "/" + fileName;
            return QFile::remove(strProFilePath);
        }
    }
    return true;
}

//获取一个HRuleFile对象，传递给m_pFrame
HRuleFile* HRuleDoc::getRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType,quint16 wRuleID, quint8 btRuleType, QString &strFormula)
{
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

    RULEPARAM* ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = wStationNo;
    //ruleParam->wPointType = wPointType;
    ruleParam->wPointNo = wPointNo;

    if(m_lpRuleDataCallBack)
    {
        if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
        {
            ruleParam->wPointType = wPointType;
            m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
        }
        else if(m_btAppType == TYPE_APP_LOCK)
        {
            ruleParam->wProtectNo = wPointType;
            m_lpRuleDataCallBack(WM_GIN_GETDBINFO,ruleParam);
        }
    }

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
        wProtID = ruleParam->wProtectNo;
        wPtNo = ruleParam->wPointNo;
        wPtTypeID = ruleParam->wPointType;
    }
    //还有其他的----huangw


    //对应厂站/间隔/测点来获取具体的规则信息
    HStationRule* stationRule = (HStationRule*)m_StationRuleList.findStationRule(wStID);
    if(!stationRule)
    {
        stationRule = new HStationRule;
        stationRule->m_wStationNo = wStID;
        stationRule->m_strStationName = strStationName;
        QString strRuleName = QString("%1_%2.fma").arg(strStationName).arg(wStationNo);
        stationRule->m_strRuleFileName = strRuleName;
        m_StationRuleList.addStationRule(stationRule);
    }

    HProtectRule* protectRule = stationRule->protectRule(wProtID);//间隔，装置
    if(!protectRule)
    {
        protectRule = new HProtectRule;
        protectRule->m_wStationNo = wStID;
        protectRule->m_wProtectNo = wProtID;
        protectRule->m_strDeviceName = strProtectName;
        stationRule->addProtectRule(protectRule);
    }

    HPointRule* pointRule = protectRule->pointRule(wPtTypeID,wPtNo);
    if(!pointRule)
    {
        pointRule = new HPointRule;
        pointRule->m_wStationNo = wStID;
        pointRule->m_wProtectNo = wProtID;
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

    HRuleFile* pRuleFile = (HRuleFile*)protectRule->getRuleFileByID(wRuleFileID);
    if(!pRuleFile)
    {
        pRuleFile = new HRuleFile;
        pRuleFile->m_wRuleFileID = stationRule->generateID();
        protectRule->addRuleFile(pRuleFile);
    }

    //复制给规则文件
    pRuleFile->m_ruleFileData.wStationNo = wStID;
    pRuleFile->m_ruleFileData.wProtectNo = wProtID;
    pRuleFile->m_ruleFileData.wPointNo = wPtNo;
    pRuleFile->m_ruleFileData.btPointType = wPointType;//测点类型 遥测 遥信 遥控
    pRuleFile->m_ruleFileData.btYKType = btYKType;
    pRuleFile->m_ruleFileData.btFormulaType = btRuleType;//规则类型:普通规则、二级规则
    //pRuleFile->m_ruleFileData.wFormulaID = ;//测点分/合等规则ID号

    QString strYKRule;//遥控规则类型：分、合、检修分、检修合
    if(CTRL_OPEN == btYKType)
    {
        strYKRule = "分规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wOpenFormulaID;
        pointRule->m_wOpenRuleFileID = pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_CLOSE == btYKType)
    {
        strYKRule = "合规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wCloseFormulaID;
        pointRule->m_wCloseRuleFileID = pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_JXOPEN == btYKType)
    {
        strYKRule = "检修分规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXOpenFormulaID;
        pointRule->m_wJXOpenRuleFileID = pRuleFile->m_wRuleFileID;
    }
    else if(CTRL_JXCLOSE == btYKType)
    {
        strYKRule = "检修合规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXCloseFormulaID;
        pointRule->m_wJXCloseRuleFileID = pRuleFile->m_wRuleFileID;
    }

    pRuleFile->m_strRuleName = QString("%1%2%3").arg(strStationName).arg(strPointName).arg(strYKRule);

    //读取完还要刷新一下所有测点规则，防止站名，站地址发生改变后无法显示正确规则
    delete ruleParam;
    return pRuleFile;
    //获取了规则文件 后续就是应该在界面上显示规则文件所有图符
}

void HRuleDoc::exportAllRule(quint16 wStationNo)
{

}

bool HRuleDoc::isRuleFileExist(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8  btYKType)
{
    HStationRule* pStRule = m_StationRuleList.findStationRule(wStationNo);
    if(!pStRule)
        return false;
    HRuleFile* rf = (HRuleFile*)pStRule->ruleFile(wPointType,wPointNo,btYKType);
    if(!rf)
        return false;
    return true;
}

bool HRuleDoc::delRuleFile(quint16 wStationNo,quint16 wPointType,quint16 wPointNo,quint8 btYKType)
{
    HStationRule* pStRule = m_StationRuleList.findStationRule(wStationNo);
    if(!pStRule)
        return false;
    return pStRule->delRuleFile(wPointType,wPointNo,btYKType);
}

void HRuleDoc::changeStationID(quint16 wStNo,quint16 wNewStNo)
{
    HStationRule* pStRule = m_StationRuleList.findStationRule(wStNo);
    if(!pStRule)
        return;

    //先删除之前的规则
    if(!delRuleProFile(pStRule->m_wStationNo))
        return;
    //再修改ID
    pStRule->changeStationNo(wNewStNo);

    //必须要重新保存一下新规则文件
    saveRuleFiles();
}

void HRuleDoc::setRuleModify(bool modify)
{
    m_bModify = modify;
}

void HRuleDoc::lookRuleReport(quint16 wStation,quint16 wPointNo)
{

}
