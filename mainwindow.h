#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QAction>
#include <QToolBar>
#include <QScrollArea>
#include <QTableWidget>
#include <QTextBrowser>
#include <QDockWidget>
#include "frame.h"
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
public:
    void createDockWindows();
    void createActions();
    void createToolBar();
    
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
    void bgset_clicked();
    void gridset_clicked();
    void idset_clicked();
    void sizeset_clicked();
    void zoomin_clicked();
    void zoomout_clicked();
private:
    QScrollArea * m_pScrollArea;
    HFrame* m_pFrame;

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
};

#endif // MAINWINDOW_H
