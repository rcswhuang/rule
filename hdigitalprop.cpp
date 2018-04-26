#include "hdigitalprop.h"
#include "ui_digitalprop.h"
#include "ruleeditapi.h"
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

}
