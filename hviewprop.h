#ifndef HVIEWPROP_H
#define HVIEWPROP_H

#include <QDialog>

namespace Ui {
class HViewProp;
}
class HRuleFile;
class HViewProp : public QDialog
{
    Q_OBJECT

public:
    explicit HViewProp(HRuleFile* pRuleFile,QWidget *parent = 0);
    ~HViewProp();

public slots:
    void ok_clicked();
private:
    Ui::HViewProp *ui;
    HRuleFile* m_pRuleFile;
};

#endif // HVIEWPROP_H
