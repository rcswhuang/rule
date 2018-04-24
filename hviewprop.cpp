#include "hviewprop.h"
#include "ui_viewprop.h"
#include "rulefile.h"
HViewProp::HViewProp(HRuleFile* pRuleFile,QWidget *parent) :
    m_pRuleFile(pRuleFile),QDialog(parent),
    ui(new Ui::HViewProp)
{
    ui->setupUi(this);
    ui->wSpinBox->setValue(m_pRuleFile->m_Size.width());
    ui->hSpinBox->setValue(m_pRuleFile->m_Size.height());
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(ok_clicked()));
}

HViewProp::~HViewProp()
{
    delete ui;
}

void HViewProp::ok_clicked()
{
    m_pRuleFile->m_Size.setWidth(ui->wSpinBox->value());
    m_pRuleFile->m_Size.setHeight(ui->hSpinBox->value());
    QDialog::accept();
}
