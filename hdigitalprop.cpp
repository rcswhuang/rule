#include "hdigitalprop.h"
#include "ui_digitalprop.h"
#include "ruleeditapi.h"
#include "drawobj.h"
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;
HDigitalProp::HDigitalProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::digitalPropDlg)
{
    ui->setupUi(this);
    initDlg();
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

    //每次启动都要去组态/运行模块读一次
    HInputObj* pInputObj = (HInputObj*)m_pDrawObj;
    if(m_wStationNo == pInputObj->m_wStationID1)
    {
        m_wMode = pInputObj->m_wMode1;
        m_wPointNo = pInputObj->m_wPointID1;
        m_wPointType = pInputObj->m_btType1;
        m_wAttr = pInputObj->m_wAttr1;
        m_btInsideType = pInputObj->m_btInType1;
    }
    m_btCondition = pInputObj->m_btCondition;

    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo;
    ruleParam->wPointNo = m_wPointNo;
    ruleParam->wPointType = m_wPointType;

    ruleParam->btInsideType = TYPE_INSIDE_DIGITAL;
    if(m_lpRuleDataCallBack)
    {
        if(TYPE_APP_JK == m_btAppType || TYPE_APP_WF == m_btAppType)
        {
            m_lpRuleDataCallBack(WM_ID_GETDBINFO,ruleParam);
        }
        else if(TYPE_APP_LOCK == m_btAppType)
        {
            m_lpRuleDataCallBack(WM_GIN_GETDBINFO,ruleParam);
        }

        m_strStationName = ruleParam->strStationName;
        m_strProtectName = ruleParam->strProtectName;
        m_strPointName = ruleParam->strPointName;
        m_strAttr = ruleParam->strAttr;
        m_btInsideType = ruleParam->btInsideType;
    }
    else
    {
        m_strStationName = "";
        m_strProtectName = "";
        m_strPointName = "";
        m_strAttr = "";
    }

    ui->condComboBox->setCurrentIndex(ui->condComboBox->findData(m_btCondition));
    refreshDlg();
}

void HDigitalProp::ptSelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
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
    ui->frNameLineEdit->setText(m_strContent);
}

void HDigitalProp::okBtn_clicked()
{
    ((HInputObj*)m_pDrawObj)->m_wMode1 = m_wMode;
    ((HInputObj*)m_pDrawObj)->m_wStationID1 = m_wStationNo;
    ((HInputObj*)m_pDrawObj)->m_btType1 = m_wPointType; //类型
    ((HInputObj*)m_pDrawObj)->m_btInType1 = m_btInsideType;
    ((HInputObj*)m_pDrawObj)->m_wPointID1 = m_wPointNo;      //点号
    ((HInputObj*)m_pDrawObj)->m_wAttr1 = m_wAttr;         //点属性
    ((HInputObj*)m_pDrawObj)->m_btCondition = m_btCondition;

    ((HInputObj*)m_pDrawObj)->m_strRuleName = m_strFormula;
    ((HInputObj*)m_pDrawObj)->m_strName = m_strContent;
    QDialog::accept();
}

void HDigitalProp::cancleBtn_clicked()
{
    QDialog::reject();
}
