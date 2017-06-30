#ifndef INPUTPROP_H
#define INPUTPROP_H

#include <QDialog>

namespace Ui {
class inputPropDlg;
}

class HInputProp : public QDialog
{
    Q_OBJECT

public:
    explicit HInputProp(QWidget *parent = 0);
    ~HInputProp();

public:
    void initDlg();
public slots:
    virtual void accept();
    void selectDigitalPoint();
    void conditionSelect(int);
private:
    Ui::inputPropDlg *ui;
};

#endif // INPUTPROP_H
