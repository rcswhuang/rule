#ifndef HLOGICPROP_H
#define HLOGICPROP_H

#include <QDialog>
class HDrawObj;
namespace Ui {
class HLogicProp;
}

class HLogicProp : public QDialog
{
    Q_OBJECT

public:
    explicit HLogicProp(HDrawObj* drawObj,QWidget *parent = 0);
    ~HLogicProp();

    void initLogicProp();

public slots:
    void ok_clicked();

public:
    HDrawObj* m_pDrawObj;
private:
    Ui::HLogicProp *ui;
};

#endif // HLOGICPROP_H
