#include "inputprop.h"
#include "ui_inputprop.h"

HInputProp::HInputProp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::inputPropDlg)
{
    ui->setupUi(this);
}

HInputProp::~HInputProp()
{
    delete ui;
}

void HInputProp::initDlg()
{

}

void HInputProp::accept()
{

    QDialog::accept();
}

void HInputProp::conditionSelect(int)
{

}

void HInputProp::selectDigitalPoint()
{

}
