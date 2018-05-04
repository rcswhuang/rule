#ifndef HLOOKRULE_H
#define HLOOKRULE_H

#include <QDialog>

namespace Ui {
class LookRuleReport;
}
class HRuleDoc;
class HLookRuleReport : public QDialog
{
    Q_OBJECT

public:
    explicit HLookRuleReport(HRuleDoc* doc,QWidget *parent = 0);
    ~HLookRuleReport();

    void setPoint(quint16 wStationNo,quint16 wPointNo);
    void initLookRuleReport();
    QString splitRuleText(QString& strRuleText);
public:
    quint16 m_wStationNo;
    quint16 m_wPointNo;
    QString m_strPointRule;
    HRuleDoc* m_pRuleDoc;
private:
    Ui::LookRuleReport *ui;
};

#endif // HLOOKRULE_H
