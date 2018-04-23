#include "hbgprop.h"
#include "ui_bgprop.h"
#include "frame.h"
#include <QColorDialog>
HBgProp::HBgProp(HFrame* frame,QWidget *parent) :
    m_pFrame(frame),QDialog(parent),
    ui(new Ui::HBgProp)
{
    ui->setupUi(this);
    connect(ui->bgBtn,SIGNAL(clicked(bool)),this,SLOT(bgBtn_clicked()));
    connect(ui->gridBtn,SIGNAL(clicked(bool)),this,SLOT(gridBtn_clicked()));
    connect(ui->fillBtn,SIGNAL(clicked(bool)),this,SLOT(fillBtn_clicked()));
    connect(ui->lineBtn,SIGNAL(clicked(bool)),this,SLOT(lineBtn_clicked()));
    connect(ui->upedgeBtn,SIGNAL(clicked(bool)),this,SLOT(upedgeBtn_clicked()));
    connect(ui->downedgeBtn,SIGNAL(clicked(bool)),this,SLOT(downedgeBtn_clicked()));
    connect(ui->shadowBtn,SIGNAL(clicked(bool)),this,SLOT(shadowBtn_clicked()));
    connect(ui->setBtn,SIGNAL(clicked(bool)),this,SLOT(set_clicked()));
    connect(ui->cancleBtn,SIGNAL(clicked(bool)),this,SLOT(cancle_clicked()));
    initBgProp();
}

HBgProp::~HBgProp()
{
    delete ui;
}

void HBgProp::initBgProp()
{
    if(!m_pFrame)
         return;


    strBgClr = m_pFrame->m_strBgClr;
    strGridClr = m_pFrame->m_strGridClr;
    strFillClr = m_pFrame->m_strFillClr;
    strLineClr = m_pFrame->m_strLineClr;
    strUpedgeClr = m_pFrame->m_strUpedgeClr;
    strDownedgeClr = m_pFrame->m_strDownedgeClr;
    strShadowClr = m_pFrame->m_strShadowClr;

    QString strbkColor = QString("background-color:")+ strBgClr;
    ui->bgBtn->setStyleSheet(strbkColor);
    QString strgridColor = QString("background-color:")+ strGridClr;
    ui->gridBtn->setStyleSheet(strgridColor);
    QString strfillColor = QString("background-color:")+ strFillClr;
    ui->fillBtn->setStyleSheet(strfillColor);
    QString strlineColor = QString("background-color:")+ strLineClr;
    ui->lineBtn->setStyleSheet(strlineColor);
    QString strupedgeColor = QString("background-color:")+ strUpedgeClr;
    ui->upedgeBtn->setStyleSheet(strupedgeColor);
    QString strdownedgeColor = QString("background-color:")+ strDownedgeClr;
    ui->downedgeBtn->setStyleSheet(strdownedgeColor);
    QString strshadowColor = QString("background-color:")+ strShadowClr;
    ui->shadowBtn->setStyleSheet(strshadowColor);
}


void HBgProp::bgBtn_clicked()
{
    QColor curColor = QColor(strBgClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strBgClr = color.name(QColor::HexRgb);
    ui->bgBtn->setStyleSheet(QString("background-color:")+strBgClr);
}

void HBgProp::gridBtn_clicked()
{
    QColor curColor = QColor(strGridClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strGridClr = color.name(QColor::HexRgb);
    ui->gridBtn->setStyleSheet(QString("background-color:")+strGridClr);
}

void HBgProp::fillBtn_clicked()
{
    QColor curColor = QColor(strFillClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strFillClr = color.name(QColor::HexRgb);
    ui->fillBtn->setStyleSheet(QString("background-color:")+strFillClr);
}

void HBgProp::lineBtn_clicked()
{
    QColor curColor = QColor(strLineClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strLineClr = color.name(QColor::HexRgb);
    ui->lineBtn->setStyleSheet(QString("background-color:")+strLineClr);
}

void HBgProp::upedgeBtn_clicked()
{
    QColor curColor = QColor(strUpedgeClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strUpedgeClr = color.name(QColor::HexRgb);
    ui->upedgeBtn->setStyleSheet(QString("background-color:")+strUpedgeClr);
}

void HBgProp::downedgeBtn_clicked()
{
    QColor curColor = QColor(strDownedgeClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strDownedgeClr = color.name(QColor::HexRgb);
    ui->downedgeBtn->setStyleSheet(QString("background-color:")+strDownedgeClr);
}

void HBgProp::shadowBtn_clicked()
{
    QColor curColor = QColor(strShadowClr);
    const QColor color = QColorDialog::getColor(curColor, this, QStringLiteral("选择颜色"));
    if(!color.isValid()) return;
    strShadowClr = color.name(QColor::HexRgb);
    ui->shadowBtn->setStyleSheet(QString("background-color:")+strShadowClr);
}

void HBgProp::set_clicked()
{
    m_pFrame->m_strBgClr = strBgClr;
    m_pFrame->m_strGridClr = strGridClr;
    m_pFrame->m_strFillClr = strFillClr;
    m_pFrame->m_strLineClr = strLineClr;
    m_pFrame->m_strUpedgeClr = strUpedgeClr;
    m_pFrame->m_strDownedgeClr = strDownedgeClr;
    m_pFrame->m_strShadowClr = strShadowClr;
    QDialog::accept();
}

void HBgProp::cancle_clicked()
{
    QDialog::reject();
}
