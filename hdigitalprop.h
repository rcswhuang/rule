﻿#ifndef DIGITALPROP_H
#define DIGITALPROP_H

#include <QDialog>
#include "drawobj.h"
namespace Ui {
class digitalPropDlg;
}

class HDigitalProp : public QDialog
{
    Q_OBJECT

public:
    explicit HDigitalProp(QWidget *parent = 0);
    ~HDigitalProp();

public:
    void initDlg();
    void refreshDlg();
public slots:
    virtual void accept();
    void ptSelBtn_clicked();
    void conditionSelect(int);
    void okBtn_clicked();
    void cancleBtn_clicked();
private:
    Ui::digitalPropDlg *ui;
    HDrawObj* m_pDrawObj;

    quint8 m_btCondition;
    QString m_strAttr;
    QString m_strFormula;//装置/间隔
    QString m_strContent;

    quint16 m_wMode;
    quint16 m_wStationNo;
    quint16 m_wProtectNo;
    quint16 m_wPointNo;
    quint16 m_wPointType;//测点类型 遥测 遥信 遥控
    quint16 m_wAttr;//规则类型
    quint8 m_btInsideType;

    QString m_strStationName;
    QString m_strProtectName;
    QString m_strPointName;
};

#endif // INPUTPROP_H
