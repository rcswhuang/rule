#include "hlookrulereport.h"
#include "ui_lookrulereport.h"
#include "hruledoc.h"
HLookRuleReport::HLookRuleReport(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LookRuleReport)
{
    ui->setupUi(this);
}

HLookRuleReport::~HLookRuleReport()
{
    delete ui;
}

HLookRuleReport::initLookRuleReport()
{
    //分，合，检修分，检修合
    ui->textBrowser->clear();
    HRuleDoc *doc;
    HStationRule* stRule = doc->m_pStationRuleList.findStationRule(m_wStationNo);
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
            //点名
            m_strPointRule += stRule->m_strStationName;
            m_strPointRule += "\n ======================================= \n";
            m_strPointRule += ptRule->m_strPointName;
            m_strPointRule += "\n ----------------------------------------\n";
            HRuleFile* ruleFile = protRule->getRuleFileByID(stRule->m_wRuleFileID);
            m_strPointRule += "分规则 \n";
            if(ruleFile)
            {

            }
            m_strPointRule += "--------------------------------------- \n";
        }

    }
}
