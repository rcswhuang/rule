#ifndef HSIMULATEPROP_H
#define HSIMULATEPROP_H

#include <QDialog>
class HRuleFile;
namespace Ui {
class HSimulateProp;
}

class HSimulateProp : public QDialog
{
    Q_OBJECT

public:
    explicit HSimulateProp(HRuleFile* rulefile,QWidget *parent = 0);
    ~HSimulateProp();
public:
    void initSimulateProp();

public slots:
    void okBtn_clicked();
    void cancelBtn_clicked();

private:
    Ui::HSimulateProp *ui;
    HRuleFile* m_pRuleFile;
};

#endif // HSIMULATEPROP_H
