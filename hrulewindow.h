#ifndef HRULEWINDOW_H
#define HRULEWINDOW_H

#include <QDialog>
#include <QAction>
#include <QToolBar>
#include <QScrollArea>
#include <QTableWidget>
#include <QTextBrowser>
#include <QDockWidget>
#include <QCloseEvent>
#include "hframe.h"
namespace Ui {
class HRuleWindow;
}
class HRuleFile;
class HRuleDoc;
class HRuleWindow : public QDialog
{
    Q_OBJECT

public:
    HRuleWindow(QWidget *parent = 0);
    HRuleWindow(HRuleFile* rf,QWidget *parent = 0);
    ~HRuleWindow();
public:
    void setRuleDoc(HRuleDoc* doc);
public:
    void createDockWindows();
    void createActions();
    void createToolBar();
    bool maybeSave();
    bool save();

public slots:
    void onCreateDigitalInput();
    void onCreateAnalogueInput();
    void onCreateConnnectLine();
    void onCreateResultOutput();
    void onCreateLogicOr();
    void onCreateLogicAnd();
    //void onDrawGrid();

    void del_clicked();
    void cut_clicked();
    void copy_clicked();
    void paste_clicked();

    void simulate_clicked();

    void bgset_clicked();
    void gridset_clicked();
    void idset_clicked();
    void sizeset_clicked();
    void zoomin_clicked();
    void zoomout_clicked();

protected:
    void closeEvent(QCloseEvent *e);
private:
    QScrollArea * m_pScrollArea;
    HFrame* m_pFrame;
    HRuleFile* m_pRuleFile;
    HRuleDoc* m_pRuleDoc;

    //confirm
    QAction* okAct;
    QAction* cancelAct;
    //edit
    QAction* delAct;
    QAction* cutAct;
    QAction* copyAct;
    QAction* pasteAct;

    //attribute
    QAction* formulaAct;
    QAction* attrAct;
    QAction* idAct;
    QAction* simuAct;

    //config
    QAction* fullAct;
    QAction* bgAct;
    QAction* gridAct;
    QAction* zoominAct;
    QAction* zoomoutAct;

    //logic
    QAction* digitalPutAct;
    QAction* analogueAct;
    QAction* lineAct;
    QAction* selectAct;
    QAction* textAct;
    QAction* andAct;
    QAction* orAct;
    QAction* outAct;

    QToolBar* confirmToolBar;
    QToolBar* editToolBar;
    QToolBar* attrToolBar;
    QToolBar* configToolBar;
    QToolBar* logicToolBar;

    //
    QDockWidget* rightDock;
    QTabWidget* rightTab;
    QTableWidget* attrTable;
    QTextBrowser* reportBrowser;

private:
    Ui::HRuleWindow *ui;
};

#endif // HRULEWINDOW_H
