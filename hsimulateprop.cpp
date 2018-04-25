#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hsimulateprop.h"
#include "ui_simulateprop.h"

HSimulateProp::HSimulateProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HSimulateProp)
{
    ui->setupUi(this);
}

HSimulateProp::~HSimulateProp()
{
    delete ui;
}

void HSimulateProp::initSimulateProp()
{
    QStringList headerList;
    headerList<<tr("test") << tr("值");
    ui->simTable->setHorizontalHeaderLabels(headerList);

}
