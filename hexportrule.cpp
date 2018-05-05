#include "hexportrule.h"
#include "ui_exportrule.h"
#include "hruledoc.h"
#include "rulefile.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
HExportRule::HExportRule(HRuleDoc *doc,QWidget *parent) :
    m_pRuleDoc(doc),QDialog(parent),
    ui(new Ui::exportRule)
{
    ui->setupUi(this);
}

HExportRule::~HExportRule()
{
    delete ui;
    clear();
}

void HExportRule::clear()
{
    while(ui->srcListWidget->count())
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->srcListWidget->takeItem(0);
        delete item;
    }
    ui->srcListWidget->clear();

    while(ui->desListWidget->count())
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->desListWidget->takeItem(0);
        delete item;
    }
    ui->desListWidget->clear();
}

void HExportRule::init()
{
    clear();
    if(!m_pRuleDoc || !m_pRuleDoc->m_pStationRuleList)
        return;
    for(int i = 0; i < m_pRuleDoc->m_pStationRuleList->count();i++)
    {
        HStationRule* pStRule = (HStationRule*)m_pRuleDoc->m_pStationRuleList->at(i);
        if(!pStRule)
        {
            QListWidgetItem* item = new QListWidgetItem;
            item->setText(pStRule->m_strStationName);
            item->setData(Qt::UserRole,pStRule->m_wStationNo);
            ui->srcListWidget->addItem(item);
        }
    }
}

void HExportRule::allexBtn_clicked()
{
    while(ui->srcListWidget->count())
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->srcListWidget->takeItem(0);
        ui->desListWidget->addItem(item);
    }
}

void HExportRule::oneexBtn_clicked()
{
    if(ui->srcListWidget->count()>0)
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->srcListWidget->takeItem(ui->srcListWidget->currentIndex());
        ui->desListWidget->addItem(item);
    }
}

void HExportRule::allimBtn_clicked()
{
    while(ui->srcListWidget->count())
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->desListWidget->takeItem(0);
        ui->desListWidget->addItem(item);
    }
}

void HExportRule::oneimBtn_clicked()
{
    if(ui->desListWidget->count()>0)
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->desListWidget->takeItem(ui->desListWidget->currentIndex());
        ui->srcListWidget->addItem(item);
    }
}

void HExportRule::exPathBtn_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("保存规则文件"),"/", tr( "CSV files(*.csv);;Text files (*.txt)"));
    ui->exPathLineEdit->setText(fileName);
}

void HExportRule::exBtn_clicked()
{
    if(ui->desListWidget->count() == 0)
        return;
    QString fileName = ui->exPathLineEdit->text();
    QFile file(fileName);
    if(!file.exists() || !file.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    QString strAllText;
    for(int i = 0; i < ui->desListWidget->count();i++)
    {
        QListWidgetItem* item = (QListWidgetItem*)ui->desListWidget->item(i);
        if(item)
        {
            HStationRule* pStRule = m_pRuleDoc->m_pStationRuleList->findStationRule(item->data(Qt::UserRole).toUInt());
            if(pStRule)
            {
                QString strStRuleText = pStRule->getRuleFileText();
                strAllText + strStRuleText;
            }
        }
    }

    QTextStream stream(&file);
    stream<<strAllText;
    file.close();

    QMessageBox::information(NULL,QStringLiteral("提示"),QStringLiteral("导出成功"),QMessageBox::Yes);
}

void HExportRule::exCancel_clicked()
{
    QDialog::reject();
}
