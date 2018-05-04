#ifndef HBGPROP_H
#define HBGPROP_H

#include <QDialog>
class HFrame;
namespace Ui {
class HBgProp;
}

class HBgProp : public QDialog
{
    Q_OBJECT

public:
    explicit HBgProp(HFrame* frame,QWidget *parent = 0);
    ~HBgProp();
    void initBgProp();

public slots:
    void bgBtn_clicked();
    void gridBtn_clicked();
    void fillBtn_clicked();
    void lineBtn_clicked();
    void upedgeBtn_clicked();
    void downedgeBtn_clicked();
    void shadowBtn_clicked();
    void set_clicked();
    void cancel_clicked();
    void default_clicked();
public:
    HFrame* m_pFrame;
    QString strBgClr;
    QString strGridClr;
    QString strFillClr; //填充色
    QString strLineClr; //线条颜色
    //QString m_clrText; //文字颜色
    QString strUpedgeClr; //上边框颜色
    QString strDownedgeClr; //下边框颜色
    QString strShadowClr; //阴影颜色
private:
    Ui::HBgProp *ui;
};

#endif // HBGPROP_H
