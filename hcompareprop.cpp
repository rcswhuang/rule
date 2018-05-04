#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hcompareprop.h"
#include "ui_compareprop.h"
#include "ruleeditapi.h"
#include "drawobj.h"
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
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(okBtn_clicked()));
    connect(ui->cancelBtn,SIGNAL(clicked(bool)),this,SLOT(cancel_clicked()));
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

    //初始化
    //每次启动都要去组态/运行模块读一次
    HInputObj* pInputObj = (HInputObj*)m_pDrawObj;
    m_btCondition = pInputObj->m_btCondition;//大于 小于
    m_btCompType = pInputObj->m_btCompType;
    m_fConstValue = pInputObj->m_fCompValue;//常量
    ui->compComboBox->setCurrentIndex(ui->compComboBox->findData(m_btCondition));
    ui->comp2TypeComboBox->setCurrentIndex(ui->comp2TypeComboBox->findData(m_btCompType));
    strConst = QString("%1").arg(m_fConstValue);
    ui->comp2LineEdit->setText(strConst);


    //获取比较值1
    if(m_wStationNo1 == pInputObj->m_wStationID1)
    {
        m_wMode1 = pInputObj->m_wMode1;
        m_wPointNo1 = pInputObj->m_wPointID1;
        m_wPointType1 = pInputObj->m_btType1;
        m_wAttr1 = pInputObj->m_wAttr1;
        m_btInsideType1 = pInputObj->m_btInType1;
    }
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo1;
    ruleParam->wPointNo = m_wPointNo1;
    ruleParam->btPointType = m_wPointType1;
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

        m_strStationName1 = ruleParam->strStationName;
        m_strProtectName1 = ruleParam->strProtectName;
        m_strPointName1 = ruleParam->strPointName;
        m_strAttr1 = ruleParam->strAttr;
        m_btInsideType1 = ruleParam->btInsideType;
    }
    else
    {
        m_strStationName1 = "";
        m_strProtectName1 = "";
        m_strPointName1 = "";
        m_strAttr1 = "";
    }

    //获取比较值2
    if(m_wStationNo2 == pInputObj->m_wStationID2)
    {
        m_wMode2 = pInputObj->m_wMode2;
        m_wPointNo2 = pInputObj->m_wPointID2;
        m_wPointType2 = pInputObj->m_btType2;
        m_wAttr1 = pInputObj->m_wAttr2;
        m_btInsideType2 = pInputObj->m_btInType2;
    }
    //RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo2;
    ruleParam->wPointNo = m_wPointNo2;
    ruleParam->btPointType = m_wPointType2;
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

        m_strStationName2 = ruleParam->strStationName;
        m_strProtectName2 = ruleParam->strProtectName;
        m_strPointName2 = ruleParam->strPointName;
        m_strAttr2 = ruleParam->strAttr;
        m_btInsideType2 = ruleParam->btInsideType;
    }
    else
    {
        m_strStationName2 = "";
        m_strProtectName2 = "";
        m_strPointName2 = "";
        m_strAttr2 = "";
    }
    refreshCompareProp();
}

void HCompareProp::comp1SelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo1;
    ruleParam->wPointNo = m_wPointNo1;
    ruleParam->btPointType = m_wPointType1;
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
            m_wPointType1 = ruleParam->btPointType;
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
    refreshCompareProp();
}

void HCompareProp::comp2SelBtn_clicked()
{
    RULEPARAM *ruleParam = new RULEPARAM;
    memset(ruleParam,0,sizeof(RULEPARAM));
    ruleParam->wStationNo = m_wStationNo2;
    ruleParam->wPointNo = m_wPointNo2;
    ruleParam->btPointType = m_wPointType2;
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
            m_wPointType2 = ruleParam->btPointType;
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
    refreshCompareProp();
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
    m_btCompType = btCompareType;
    refreshCompareProp();
}

void HCompareProp::comp2LineEdit_textEdited(const QString& value)
{
    m_fConstValue = value.toFloat();
    ui->textBrowser->setText(value);
    refreshCompareProp();
}

void HCompareProp::refreshCompareProp()
{
    //textBrowser显示内容=[m_comp1LineEdit] + ">" + m_comp2LineEdit
    //m_comp2LineEdit 如果是遥测 则变成[m_comp2LineEdit]
    //QString strTextBrowser = "(" + ")";
    QString strComp1;
    QString strComp2;
    quint8 nComp2 = m_btCompType;//ui->comp2TypeComboBox->currentData().toUInt();
    if(m_btAppType == TYPE_APP_JK || m_btAppType == TYPE_APP_WF)
    {
        strComp1 = "["+m_strStationName1+"."+m_strPointName1+"."+m_strAttr1+"]";
        ui->comp1LineEdit->setText(strComp1);

        if(TYPE_COMPARE_CONST == nComp2)
        {
            strComp2 = QString("%1").arg(m_fConstValue);
        }
        else
        {
            strComp2 = "["+m_strStationName2+"."+m_strPointName2+"."+m_strAttr2+"]";
        }
        ui->comp2LineEdit->setText(strComp2);
    }
    else if(m_btAppType == TYPE_APP_LOCK)
    {

    }

    QString strCond;//条件表达式
    quint8 nCond = ui->compComboBox->currentData().toUInt();
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
    m_strFormula = "("+strComp1+strCond+strComp2+")";
    m_strContent = strComp1 + strCond + strComp2;
    ui->textBrowser->setText(m_strContent);
}

void HCompareProp::okBtn_clicked()
{

    ((HInputObj*)m_pDrawObj)->m_wMode1 = m_wMode1;
    ((HInputObj*)m_pDrawObj)->m_wStationID1 = m_wStationNo1;
    ((HInputObj*)m_pDrawObj)->m_btType1 = m_wPointType1; //类型
    ((HInputObj*)m_pDrawObj)->m_btInType1 = m_btInsideType1;
    ((HInputObj*)m_pDrawObj)->m_wPointID1 = m_wPointNo1;      //点号
    ((HInputObj*)m_pDrawObj)->m_wAttr1 = m_wAttr1;         //点属性

    ((HInputObj*)m_pDrawObj)->m_wMode2 = m_wMode2;
    ((HInputObj*)m_pDrawObj)->m_wStationID2 = m_wStationNo2;
    ((HInputObj*)m_pDrawObj)->m_btType2 = m_wPointType2; //类型
    ((HInputObj*)m_pDrawObj)->m_btInType2 = m_btInsideType2;
    ((HInputObj*)m_pDrawObj)->m_wPointID2 = m_wPointNo2;      //点号
    ((HInputObj*)m_pDrawObj)->m_wAttr2 = m_wAttr2;         //点属性

    ((HInputObj*)m_pDrawObj)->m_btCondition = ui->compComboBox->currentData().toUInt();
    ((HInputObj*)m_pDrawObj)->m_btCompType = m_btCompType;
    ((HInputObj*)m_pDrawObj)->m_fCompValue = m_fConstValue;

    ((HInputObj*)m_pDrawObj)->m_strRuleName = m_strFormula;
    ((HInputObj*)m_pDrawObj)->m_strName = m_strContent;
    QDialog::accept();
}

void HCompareProp::cancel_clicked()
{
    QDialog::reject();
}
