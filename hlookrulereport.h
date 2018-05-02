#ifndef HLOOKRULE_H
#define HLOOKRULE_H

#include <QDialog>

namespace Ui {
class LookRuleReport;
}

class HLookRuleReport : public QDialog
{
    Q_OBJECT

public:
    explicit HLookRuleReport(QWidget *parent = 0);
    ~HLookRuleReport();

    void initLookRuleReport();

public:
    quint16 m_wStationNo;
    quint16 m_wPointNo;
    QString m_strPointRule;
private:
    Ui::LookRuleReport *ui;
};

#endif // HLOOKRULE_H
