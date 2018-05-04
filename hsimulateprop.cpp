#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hsimulateprop.h"
#include "ui_simulateprop.h"
#include <QComboBox>
#include <QTableWidgetItem>
#include "rulefile.h"
HSimulateProp::HSimulateProp(HRuleFile* rulefile,QWidget *parent) :
    m_pRuleFile(rulefile),QDialog(parent),
    ui(new Ui::HSimulateProp)
{
    ui->setupUi(this);
    initSimulateProp();
}

HSimulateProp::~HSimulateProp()
{
    delete ui;
}

void HSimulateProp::initSimulateProp()
{
    connect(ui->okBtn,SIGNAL(clicked(bool)),this,SLOT(okBtn_clicked()));
    connect(ui->cancelBtn,SIGNAL(clicked(bool)),this,SLOT(cancleBtn_clicked()));
    ui->simTable->setColumnCount(2);
    ui->simTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QStringList headerList;
    headerList<<tr("输入") << tr("值");
    ui->simTable->setHorizontalHeaderLabels(headerList);


    //获取遥测遥信对象的个数
    int nRowCount = 0;
    for(int i = 0; i < m_pRuleFile->m_drawObjList.count();i++)
    {
        HDrawObj* obj = (HDrawObj*)m_pRuleFile->m_drawObjList[i];
        if(TYPE_INPUT == obj->getObjType())
            nRowCount++;
    }

    ui->simTable->setRowCount(nRowCount);
    for(int i = 0; i < m_pRuleFile->m_drawObjList.count();i++)
    {
        HDrawObj* obj = (HDrawObj*)m_pRuleFile->m_drawObjList[i];
        if(TYPE_INPUT == obj->getObjType())
        {
            QTableWidgetItem* item1 = new QTableWidgetItem();
            item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
            item1->setText(obj->m_strName);
            item1->setData(Qt::UserRole,obj->m_dwID);
            ui->simTable->setItem(i,0,item1);


            QComboBox* box = new QComboBox;
            box->addItem("FALSE",false);
            box->addItem("TRUE",true);
            box->setCurrentIndex(box->findData(obj->m_bOutValue));
            ui->simTable->setCellWidget(i,1,box);
        }
    }
}

void HSimulateProp::okBtn_clicked()
{
    for(int i = 0; i < m_pRuleFile->m_drawObjList.count();i++)
    {
        HDrawObj* obj = (HDrawObj*)m_pRuleFile->m_drawObjList[i];
        if(TYPE_INPUT == obj->getObjType())
        {
            QComboBox *box = ( QComboBox*)ui->simTable->cellWidget(i,1);
            bool bValue = box->currentData().toBool();
            obj->m_bOutValue = bValue;
        }
    }
    QDialog::accept();
}

void HSimulateProp::cancelBtn_clicked()
{
    QDialog::reject();
}
