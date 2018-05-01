#include "hruledoc.h"
#include <QDir>
#include <QProcessEnvironment>
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;

HRuleDoc::HRuleDoc()
{

}

HRuleDoc::~HRuleDoc()
{

}

//加载所有规则
bool HRuleDoc::loadRuleFiles()
{
    //QString strRuleFile = g_strRuleFilePath + "*.FRO";
    //xxx/rule/station_0.PRO
    QDir dirRulePath(m_strRuleFilePath);
    if(!dirRulePath.exists())
        return;
    QStringList filters;
    filters<<"*.FRO";
    dirRulePath.setNameFilters(filters);
    QFileInfoList iconsFileInfoList = dirRulePath.entryInfoList(QDir::Files  | QDir::NoDotAndDotDot);
    foreach(QFileInfo info,iconsFileInfoList)
    {
        if(info.isFile())continue;
        QString strRuleFile = dirRulePath + "/" + info.fileName();
        QFile file(strRuleFile);
        if(!file.exists() || !file.open(QIODevice::ReadOnly))
            continue;
        QDataStream stream(&file);
        HStationRule* pStationRule = new HStationRule;
        if(pStationRule)
            pStationRule->readData(QDataStream::Qt_5_10,&stream);

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
    QString wfPath = QProcessEnvironment::systemEnvironment().value("wfsystem_dir");
    wfPath.append("/rule");
    QDir wfDir(wfPath);
    if(!wfDir.exists())
        wfDir.mkdir(wfPath);
    //for(int i = 0; i < g_StationRuleList)
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
            m_lpRuleDataCallBack(m_btAppType,ruleParam);
        }
        else if(m_btAppType == TYPE_APP_LOCK)
        {
            ruleParam->wProtectNo = wPointType;
            m_lpRuleDataCallBack(m_btAppType,ruleParam);
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
        QString strRuleName = QString("%1_%2.FOR").arg(strStationName).arg(wStationNo);
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

    //---huangw
    protectRule->refreshRuleFileID(pointRule);
    //规则文件ID
    quint16 wRuleFileID;
    if(CTRL_OPEN == btYKType)
        wRuleFileID = pointRule->m_wOpenRuleFileID;
    else if(CTRL_CLOSE == btYKType)
        wRuleFileID = pointRule->m_wCloseRuleFileID;
    else if(CTRL_JXOPEN == btYKType)
        wRuleFileID = pointRule->m_wJXOpenRuleFileID;
    else if(CTRL_JXCLOSE == btYkTYpe)
        wRuleFileID = pointRule->m_wJXCloseRuleFileID;

    HRuleFile* pRuleFile = (HRuleFile*)protectRule->getRuleFileByID(wRuleFileID);
    if(!pRuleFile)
    {
        pRuleFile = new HRuleFile;
        pRuleFile->m_nRuleFileID = stationRule->generateID();
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
        pointRule->m_wOpenRuleFileID = pRuleFile->m_nRuleFileID;
    }
    else if(CTRL_CLOSE == btYKType)
    {
        strYKRule = "合规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wCloseFormulaID;
        pointRule->m_wCloseRuleFileID = pRuleFile->m_nRuleFileID;
    }
    else if(CTRL_JXOPEN == btYKType)
    {
        strYKRule = "检修分规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXOpenFormulaID;
        pointRule->m_wJXOpenRuleFileID = pRuleFile->m_nRuleFileID;
    }
    else if(CTRL_JXCLOSE == btYkTYpe)
    {
        strYKRule = "检修合规则";
        pRuleFile->m_ruleFileData.wFormulaID = pointRule->m_wJXCloseFormulaID;
        pointRule->m_wJXCloseRuleFileID = pRuleFile->m_nRuleFileID;
    }

    pRuleFile->m_strRuleName = QString("%1%2%3").arg(strStationName).arg(strPointName).arg(strYKRule);

    delete ruleParam;
    return pRuleFile;
    //获取了规则文件 后续就是应该在界面上显示规则文件所有图符
}
