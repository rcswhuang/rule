#include "hlogicprop.h"
#include "ui_logicprop.h"
#include "drawobj.h"
HLogicProp::HLogicProp(HDrawObj* drawObj,QWidget *parent) :
    m_pDrawObj(drawObj),QDialog(parent),
    ui(new Ui::HLogicProp)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(ok_clicked()));
    initLogicProp();
}

HLogicProp::~HLogicProp()
{
    delete ui;
}

void HLogicProp::initLogicProp()
{
    for(int i = 2; i <=30;i++)
    {
        QString strInput = QString("输入%1").arg(i);
        ui->comboBox->addItem(strInput,i);
    }

    int nInputSum = m_pDrawObj->m_nInPointSum;
    ui->comboBox->setCurrentIndex(ui->comboBox->findData(nInputSum));
}

void HLogicProp::ok_clicked()
{
    int nSum = ui->comboBox->currentData().toInt();
    if(nSum == m_pDrawObj->m_nInPointSum) return;

    int oneh = m_pDrawObj->m_pointIn[1].y() - m_pDrawObj->m_pointIn[0].y();
    m_pDrawObj->m_rectCurPos.setBottom(m_pDrawObj->m_rectCurPos.top() + oneh*(nSum+4));
    m_pDrawObj->m_nInPointSum = nSum;
    QDialog::accept();
}
