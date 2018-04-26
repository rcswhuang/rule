#include "hdigitalprop.h"
#include "ui_digitalprop.h"
#include "ruleeditapi.h"
//extern LPRULEDATACALLBACK m_lpRuleDataCallBack;

HDigitalProp::HDigitalProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::digitalPropDlg)
{
    ui->setupUi(this);
}

HDigitalProp::~HDigitalProp()
{
    delete ui;
}

void HDigitalProp::initDlg()
{
    ui->frNameLineEdit->setReadOnly(true);
    ui->stNameLineEdit->setReadOnly(true);
    ui->jgNameLineEdit->setReadOnly(true);
    ui->ptNameLineEdit->setReadOnly(true);
    ui->condComboBox->addItem("分位置",COND_OPEN);
    ui->condComboBox->addItem("合位置",COND_CLOSE);



}

void HDigitalProp::accept()
{

    QDialog::accept();
}

void HDigitalProp::conditionSelect(int)
{

}


void HDigitalProp::ptSelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleFileData,0,sizeof(RULEFILEDATA));
    ruleParam->wStationNo = m_wStationNo;
    ruleParam->wPointNo = m_wPointNo;
    ruleParam->wPointType = m_wPointType;
    ruleParam->wAttr = m_wAttr;
    ruleParam->btInsideType = TYPE_INSIDE_DIGITAL;

    //通过回调函数去调用
    if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
    {
        m_lpRuleDataCallBack(WM_SEL_POINT,ruleParam);
        m_wStationNo = ruleParam->wStationNo;
        m_wProtectNo = ruleParam->wProtectNo;
        m_wPointNo = ruleParam->wPointNo;
        m_wPointType = ruleParam->wPointType;
        m_wAttr = ruleParam->wAttr;
        m_strAttr = ruleParam->strAttr;
    }

    m_btInsideType = ruleParam->btInsideType;
    m_strStationName = ruleParam->strStationName;
    m_strProtectName = ruleParam->strProtectName;
    m_strPointName = ruleParam->strPointName;
    delete ruleParam;
    refreshDlg();
}

void HDigitalProp::refreshDlg()
{
    //获取条件
    m_btCondition = ui->condComboBox->currentData().toUInt();
    QString strCondition = ui->condComboBox->currentText();

    if(TYPE_APP_WF == m_btAppType || TYPE_APP_JK)
    {
        if(COND_CLOSE == m_btCondition)
        {
            m_strFormula = "["+m_strStationName+"."+m_strPointName+"."+m_strAttr+"]";
            m_strContent = m_strPointName + "=" +strCondition;
        }
        else
        {
            m_strFormula = "~["+m_strStationName+"."+m_strPointName+"."+m_strAttr+"]";
            m_strContent = m_strPointName + "=" +strCondition;
        }
    }
    else if(TYPE_APP_LOCK == m_btAppType)
    {
        if(COND_CLOSE == m_btCondition)
        {
            m_strFormula = "["+m_strStationName+"."+m_strProtectName+"."+m_strPointName+"]";
            m_strContent = m_strPointName + "=" +strCondition;
        }
        else
        {
            m_strFormula = "~["+m_strStationName+"."+m_strProtectName+"."+m_strPointName+"]";
            m_strContent = m_strPointName + "=" +strCondition;
        }
    }
    ui->frNameLineEdit = m_strContent;
}

void HDigitalProp::okBtn_clicked()
{
    ((HInputObj*)m_pDrawObj)->m_wMode1 = m_wMode;
    ((HInputObj*)m_pDrawObj)->m_wStationID1 = m_wStationNo;
    ((HInputObj*)m_pDrawObj)->m_wProtID1 = m_wPointType; //类型
    ((HInputObj*)m_pDrawObj)->m_btInType1 = m_btInsideType;
    ((HInputObj*)m_pDrawObj)->m_wPointID1 = m_wPointNo;      //点号
    ((HInputObj*)m_pDrawObj)->m_wAttr1 = m_wAttr;         //点属性

    ((HInputObj*)m_pDrawObj)->m_strRuleName = m_strFormula;
    ((HInputObj*)m_pDrawObj)->m_strName = m_strContent;
    QDialog::accept();
}

void HDigitalProp::cancleBtn_clicked()
{
    QDialog::reject();
}
