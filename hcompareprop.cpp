#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hcompareprop.h"
#include "ui_compareprop.h"
#include "ruleeditapi.h"
#include <QDoubleValidator>
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;


HCompareProp::HCompareProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::compareProp)
{
    ui->setupUi(this);
    initCompareProp();
}

HCompareProp::~HCompareProp()
{
    delete ui;
}


void HCompareProp::initCompareProp()
{
    connect(ui->comp1SelBtn,SIGNAL(clicked(bool)),this,SLOT(comp1SelBtn_clicked()));
    connect(ui->comp2SelBtn,SIGNAL(clicked(bool)),this,SLOT(comp2SelBtn_clicked()));
    connect(ui->comp2TypeComboBox,SIGNAL(currentIndexChanged(int)),SLOT(comp2TypeComboBox_clicked()));
    connect(ui->comp2LineEdit,SIGNAL(textEdited(QString)),this,SLOT(comp2LineEdit_textEdited(QString)));
    ui->compComboBox->addItem("大于",OP_GREATER);
    ui->compComboBox->addItem("小于",OP_LOWER);
    ui->compComboBox->addItem("等于",OP_EQUAL);
    ui->compComboBox->addItem("大于等于",OP_GEQUAL);
    ui->compComboBox->addItem("小于等于",OP_LEQUAL);
    ui->compComboBox->addItem("不等于",OP_NEQUAL);
    ui->compComboBox->setCurrentIndex(0);
    ui->comp2TypeComboBox->addItem("常数",TYPE_COMPARE_CONST);
    ui->comp2TypeComboBox->addItem("遥测",TYPE_COMPARE_ANALOGUE);
    ui->comp2LineEdit->setValidator(new QDoubleValidator(-999999.0,999999.0,6));
    m_fConstValue = 0.000000;
    QString strConst = QString("%1").arg((double)m_fConstValue);
    ui->comp2LineEdit->setText(strConst);
    ui->comp2TypeComboBox->setCurrentIndex(0);

}

void HCompareProp::comp1SelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo1;
    ruleParam->wPointNo = m_wPointNo1;
    ruleParam->wPointType = m_wPointType1;
    ruleParam->wAttr = m_wAttr1;
    ruleParam->btInsideType = TYPE_INSIDE_ANALOGUE;

    //通过回调函数去调用
    if(m_lpRuleDataCallBack)
    {
        if(m_btAppType == TYPE_APP_LOCK)
        {
            //联锁状态 厂站 装置ID 装置GIN号 无测点类型
            m_lpRuleDataCallBack(WM_SEL_POINT,ruleParam);
            m_wStationNo1 = ruleParam->wStationNo;
            m_wProtectNo1 = ruleParam->wProtectNo;
            m_wPointNo1 = ruleParam->wPointNo;
            m_wAttr1 = ruleParam->wAttr;
            m_strAttr1 = ruleParam->strAttr;
        }
        else if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
        {
            //监控五防，厂站，类型，点号，无装置ID
            m_lpRuleDataCallBack(WM_SEL_POINT,ruleParam);
            m_wStationNo1 = ruleParam->wStationNo;
            m_wPointNo1 = ruleParam->wPointNo;
            m_wPointType1 = ruleParam->wPointType;
            m_wAttr1 = ruleParam->wAttr;
            m_strAttr1 = ruleParam->strAttr;
        }

        m_btInsideType1 = ruleParam->btInsideType;
        m_strStationName1 = ruleParam->strStationName;
        m_strProtectName1 = ruleParam->strProtectName;
        m_strPointName1 = ruleParam->strPointName;

        ui->comp1LineEdit->setText(m_strPointName1);

        delete ruleParam;
    }
    //refreshDlg();
}

void HCompareProp::comp2SelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo2;
    ruleParam->wPointNo = m_wPointNo2;
    ruleParam->wPointType = m_wPointType2;
    ruleParam->wAttr = m_wAttr2;
    ruleParam->btInsideType = TYPE_INSIDE_ANALOGUE;

    //通过回调函数去调用
    if(m_lpRuleDataCallBack)
    {
        if(m_btAppType == TYPE_APP_LOCK)
        {
            m_lpRuleDataCallBack(WM_SEL_POINT,ruleParam);
            m_wStationNo2 = ruleParam->wStationNo;
            m_wProtectNo2 = ruleParam->wProtectNo;
            m_wPointNo2 = ruleParam->wPointNo;
            m_wAttr2 = ruleParam->wAttr;
            m_strAttr2 = ruleParam->strAttr;
        }
        else if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
        {
            m_lpRuleDataCallBack(WM_SEL_POINT,ruleParam);
            m_wStationNo2 = ruleParam->wStationNo;
            m_wPointNo2 = ruleParam->wPointNo;
            m_wPointType2 = ruleParam->wPointType;
            m_wAttr2 = ruleParam->wAttr;
            m_strAttr2 = ruleParam->strAttr;
        }

        m_btInsideType2 = ruleParam->btInsideType;
        m_strStationName2 = ruleParam->strStationName;
        m_strProtectName2 = ruleParam->strProtectName;
        m_strPointName2 = ruleParam->strPointName;
        ui->comp2LineEdit->setText(m_strPointName1);
        delete ruleParam;
    }
}

void HCompareProp::comp2TypeComboBox_clicked()
{
    quint8 btCompareType = ui->comp2TypeComboBox->currentData().toUInt();
    if(TYPE_COMPARE_ANALOGUE == btCompareType)
    {
        ui->comp2SelBtn->setVisible(true);
        ui->comp2LineEdit->setEnabled(false);
        ui->comp2LineEdit->setText(m_strPointName2);

    }
    else if(TYPE_COMPARE_CONST == btCompareType)
    {
        ui->comp2SelBtn->setVisible(false);
        ui->comp2LineEdit->setEnabled(true);
        QString strConst = QString("%1").arg((double)m_fConstValue);
        ui->comp2LineEdit->setText(strConst);
    }

}

void HCompareProp::comp2LineEdit_textEdited(const QString& value)
{
    m_fConstValue = value.toFloat();
    ui->textBrowser->setText(value);
}

void HCompareProp::refreshCompareProp()
{
    //textBrowser显示内容=[m_comp1LineEdit] + ">" + m_comp2LineEdit
    //m_comp2LineEdit 如果是遥测 则变成[m_comp2LineEdit]
    //QString strTextBrowser = "(" + ")";
}


