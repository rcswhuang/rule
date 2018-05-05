#ifndef HEXPORTRULE_H
#define HEXPORTRULE_H

#include <QDialog>

namespace Ui {
class exportRule;
}
class HStationRuleList;
class HRuleDoc;
class HExportRule : public QDialog
{
    Q_OBJECT

public:
    explicit HExportRule(HRuleDoc*doc,QWidget *parent = 0);
    ~HExportRule();
    void init();
    void clear();

public slots:
    void allexBtn_clicked();
    void oneexBtn_clicked();
    void allimBtn_clicked();
    void oneimBtn_clicked();

    void exPathBtn_clicked();
    void exBtn_clicked();
    void exCancel_clicked();
private:
    Ui::exportRule *ui;
    HRuleDoc* m_pRuleDoc;
    HStationRuleList* m_pStationRuleList;
};

#endif // HEXPORTRULE_H
