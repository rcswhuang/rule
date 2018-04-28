#ifndef HCOMPAREPROP_H
#define HCOMPAREPROP_H

#include <QDialog>
class HDrawObj;
namespace Ui {
class compareProp;
}

class HCompareProp : public QDialog
{
    Q_OBJECT

public:
    explicit HCompareProp(QWidget *parent = 0);
    ~HCompareProp();

public:
    void initCompareProp();
    void refreshCompareProp();
public slots:
    void comp1SelBtn_clicked();
    void comp2SelBtn_clicked();
    void comp2TypeComboBox_clicked();
    void comp2LineEdit_textEdited(const QString& value);

private:
    Ui::compareProp *ui;
    HDrawObj* m_pDrawObj;
    quint8 m_btCompType;//测点1是与测点2或常数比较
    quint8 m_btCondition;
    QString m_strAttr1; //测点1属性
    QString m_strAttr2; //测点2属性
    QString m_strFormula;
    QString m_strContent;


    //测点1
    quint16 m_wMode1;
    quint16 m_wStationNo1;
    quint16 m_wProtectNo1;//装置/间隔
    quint16 m_wPointNo1;
    quint16 m_wPointType1;//测点类型 遥测 遥信 遥控
    quint16 m_wAttr1;//规则类型
    quint8 m_btInsideType1;

    //测点2
    quint16 m_wMode2;
    quint16 m_wStationNo2;
    quint16 m_wProtectNo2;//装置/间隔
    quint16 m_wPointNo2;
    quint16 m_wPointType2;//测点类型 遥测 遥信 遥控
    quint16 m_wAttr2;//规则类型
    quint8 m_btInsideType2;

    //遥测1
    QString m_strStationName1;
    QString m_strProtectName1;
    QString m_strPointName1;

    //遥测2
    QString m_strStationName2;
    QString m_strProtectName2;
    QString m_strPointName2;

    //常量
    float m_fConstValue;
};

#endif // HCOMPAREPORP_H
