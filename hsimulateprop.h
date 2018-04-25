#ifndef HSIMULATEPROP_H
#define HSIMULATEPROP_H

#include <QDialog>

namespace Ui {
class HSimulateProp;
}

class HSimulateProp : public QDialog
{
    Q_OBJECT

public:
    explicit HSimulateProp(QWidget *parent = 0);
    ~HSimulateProp();
public:
    void initSimulateProp();

private:
    Ui::HSimulateProp *ui;
};

#endif // HSIMULATEPROP_H
