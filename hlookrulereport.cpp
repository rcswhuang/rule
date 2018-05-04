#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hlookrulereport.h"
#include "ui_lookrulereport.h"
#include "hruledoc.h"
HLookRuleReport::HLookRuleReport(HRuleDoc* doc,QWidget *parent) :m_pRuleDoc(doc),
    QDialog(parent),
    ui(new Ui::LookRuleReport)
{
    ui->setupUi(this);
    m_strPointRule = "";
}

HLookRuleReport::~HLookRuleReport()
{
    delete ui;
}

void HLookRuleReport::setPoint(quint16 wStationNo, quint16 wPointNo)
{
    m_wStationNo = wStationNo;
    m_wPointNo = wPointNo;
}

void HLookRuleReport::initLookRuleReport()
{
    //分，合，检修分，检修合
    ui->textBrowser->clear();
    QString strStationName,strPointName;
    QString strOpenRule,strCloseRule,strJXOpenRule,strJXCloseRule;
    HStationRule* stRule = (HStationRule*)m_pRuleDoc->m_pStationRuleList->findStationRule(m_wStationNo);
    if(!stRule) return;
    for(int i = 0;i < stRule->m_pProtRuleList.count();i++)
    {
        HProtectRule* protRule = (HProtectRule*)stRule->m_pProtRuleList[i];
        if(!protRule) continue;
        for(int k = 0; k < protRule->m_pPointRuleList.count();k++)
        {
            HPointRule* ptRule = (HPointRule*)protRule->m_pPointRuleList[i];
            if(!ptRule || ptRule->m_wPointNo != m_wPointNo)
                continue;
            strStationName = stRule->m_strStationName;
            strPointName = ptRule->m_strPointName;
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
        }
    }

    //开始显示规则
    m_strPointRule += strStationName;
    m_strPointRule += "\n ======================================= \n";
    m_strPointRule += strPointName;
    m_strPointRule += "\n ----------------------------------------\n";
    m_strPointRule += "分规则: \n";
    if(strOpenRule.isEmpty())
        m_strPointRule += "\n";
    else
        m_strPointRule += splitRuleText(strOpenRule);
    m_strPointRule += "--------------------------------------- \n";

    m_strPointRule += "合规则: \n";
    if(strOpenRule.isEmpty())
        m_strPointRule += "\n";
    else
        m_strPointRule += splitRuleText(strCloseRule);
    m_strPointRule += "--------------------------------------- \n";

    m_strPointRule += "备用分规则: \n";
    if(strOpenRule.isEmpty())
        m_strPointRule += "\n";
    else
        m_strPointRule += splitRuleText(strJXOpenRule);
    m_strPointRule += "--------------------------------------- \n";

    m_strPointRule += "备用合规则: \n";
    if(strOpenRule.isEmpty())
        m_strPointRule += "\n";
    else
        m_strPointRule += splitRuleText(strJXCloseRule);
    m_strPointRule += "--------------------------------------- \n";
    m_strPointRule += "======================================= \n";

    ui->textBrowser->setPlainText(m_strPointRule);
}

QString HLookRuleReport::splitRuleText(QString& strRuleText)
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
