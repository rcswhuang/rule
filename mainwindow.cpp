﻿#include "mainwindow.h"
#include "drawtool.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include "hbgprop.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createActions();
    createToolBar();
    //createDockWindows();

    m_pScrollArea = new QScrollArea(this);
    m_pFrame = new HFrame(m_pScrollArea);
    m_pScrollArea->setWidget(m_pFrame);
    m_pScrollArea->widget()->resize(1200,1000);
    setCentralWidget(m_pScrollArea);
    resize(800,600);
   // connect(m_pScrollArea,SIGNAL())
    onDrawGrid();
}

MainWindow::~MainWindow()
{

}

void MainWindow::createActions()
{
    setIconSize(QSize(16,16));
    okAct = new QAction(QIcon(":image/ok.png"),tr("Ok"),this);
    cancelAct = new QAction(QIcon(":image/cancel.png"),tr("Cancel"),this);

    delAct = new QAction(QIcon(":image/edit_del.png"),tr("&Delete"),this);
    cutAct = new QAction(QIcon(":image/edit_cut.png"),tr("&Cut"),this);
    copyAct = new QAction(QIcon(":image/edit_copy.png"),tr("&Copy"),this);
    pasteAct = new QAction(QIcon(":image/edit_paste.png"),tr("&Paste"),this);

    formulaAct = new QAction(QIcon(":image/formula.png"),tr("生成公式"),this);
    attrAct = new QAction(QIcon(":image/report.png"),tr("规则报告"),this);
    idAct = new QAction(QIcon(":image/ID.png"),tr("ID"),this);
    simuAct = new QAction(QIcon(":image/simulation.png"),tr("公式仿真"),this);

    fullAct = new QAction(QIcon(":image/full.png"),tr("调整大小"),this);
    bgAct = new QAction(QIcon(":image/bgcolor.png"),tr("颜色设置"),this);
    connect(bgAct,SIGNAL(triggered(bool)),this,SLOT(bgset_clicked()));
    gridAct = new QAction(QIcon(":image/grid.png"),tr("显示网格"),this);
    zoominAct = new QAction(QIcon(":image/zoom_in.png"),tr("放大"),this);
    zoomoutAct = new QAction(QIcon(":image/zoom_out.png"),tr("缩小"),this);


    digitalPutAct = new QAction(QIcon(":/image/digital.png"),tr("&Digital"),this);
    connect(digitalPutAct,SIGNAL(triggered()),this,SLOT(onCreateDigitalInput()));

    analogueAct = new QAction(QIcon(":image/analogue.png"),tr("&Analogue"),this);
    connect(analogueAct,SIGNAL(triggered()),this,SLOT(onCreateAnalogueInput()));
    selectAct = new QAction(QIcon(":image/select.png"),tr("&Select"),this);
    textAct = new QAction(QIcon(":image/text.png"),tr("&Delete"),this);
    lineAct = new QAction(QIcon(":image/line.png"),tr("&Delete"),this);
    connect(lineAct,SIGNAL(triggered()),this,SLOT(onCreateConnnectLine()));

    andAct = new QAction(QIcon(":image/and.png"),tr("&And"),this);
    connect(andAct,SIGNAL(triggered()),this,SLOT(onCreateLogicAnd()));
    orAct = new QAction(QIcon(":image/or.png"),tr("&OR"),this);
    connect(orAct,SIGNAL(triggered()),this,SLOT(onCreateLogicOr()));
    outAct = new QAction(QIcon(":image/out.png"),tr("&Output"),this);
    connect(outAct,SIGNAL(triggered()),this,SLOT(onCreateResultOutput()));
}

void MainWindow::createDockWindows()
{
    rightDock = new QDockWidget(this);
    rightDock->setAllowedAreas(Qt::RightDockWidgetArea);
    QWidget* widget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(2);
    layout->setContentsMargins(2, 2, 2, 2);
    rightTab = new QTabWidget(rightDock);
    rightTab->setTabPosition(QTabWidget::South);

    attrTable = new QTableWidget(rightTab);
    attrTable->setColumnCount(2);
    QStringList hLabels;
    hLabels<<"属性"<<"值";
    attrTable->setHorizontalHeaderLabels(hLabels);
    attrTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    rightTab->addTab(attrTable,"属性设置");


    reportBrowser = new QTextBrowser(rightTab);
    reportBrowser->setReadOnly(true);
    rightTab->insertTab(1,reportBrowser,"规则报告");
    layout->addWidget(rightTab);
    //rightDock->setLayout(layout);
    rightDock->setWidget(widget);
    rightDock->setMinimumWidth(200);
    addDockWidget(Qt::RightDockWidgetArea,rightDock);

}

void MainWindow::createToolBar()
{

    confirmToolBar = new QToolBar;
    confirmToolBar->setIconSize(QSize(20,20));
    confirmToolBar->setMovable(false);
    confirmToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    confirmToolBar->addAction(okAct);
    confirmToolBar->addAction(cancelAct);
    addToolBar(Qt::TopToolBarArea,confirmToolBar);
    addToolBarBreak();

    editToolBar = new QToolBar;
    editToolBar->setIconSize(QSize(20,20));
    editToolBar->setMovable(false);
    editToolBar->addAction(delAct);
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    addToolBar(editToolBar);

    attrToolBar = new QToolBar;
    attrToolBar->setIconSize(QSize(20,20));
    attrToolBar->setMovable(false);
    attrToolBar->addAction(formulaAct);
    attrToolBar->addAction(attrAct);
    attrToolBar->addAction(idAct);
    attrToolBar->addAction(simuAct);
    addToolBar(attrToolBar);

    configToolBar = new QToolBar;
    configToolBar->setIconSize(QSize(20,20));
    configToolBar->setMovable(false);
    configToolBar->addAction(formulaAct);
    configToolBar->addAction(bgAct);
    configToolBar->addAction(gridAct);
    configToolBar->addAction(zoominAct);
    configToolBar->addAction(zoomoutAct);
    addToolBar(configToolBar);

    logicToolBar = new QToolBar;
    logicToolBar->setIconSize(QSize(20,20));
    logicToolBar->setMovable(false);
    logicToolBar->addAction(selectAct);
    logicToolBar->addAction(lineAct);
    logicToolBar->addAction(textAct);
    logicToolBar->addAction(andAct);
    logicToolBar->addAction(orAct);
    logicToolBar->addAction(digitalPutAct);
    logicToolBar->addAction(analogueAct);
    logicToolBar->addAction(outAct);
    addToolBar(Qt::LeftToolBarArea,logicToolBar);
}

void MainWindow::onCreateDigitalInput()
{
    //m_pFrame->setDrawShape(enumInputPOINT);
    HDrawTool::drawShape = enumInputDIGITAL;
}

void MainWindow::onCreateAnalogueInput()
{
    HDrawTool::drawShape = enumInputANALOGUE;
}

void MainWindow::onCreateConnnectLine()
{
    HDrawTool::drawShape = enumConnect;
}

void MainWindow::onCreateResultOutput()
{
    HDrawTool::drawShape = enumOutRESUTL;
}

void MainWindow::onCreateLogicOr()
{
    HDrawTool::drawShape = enumLogicOR;
}

void MainWindow::onCreateLogicAnd()
{
    HDrawTool::drawShape = enumLogicAND;
}

void MainWindow::onDrawGrid()
{
    m_pFrame->m_bGrid = true;
    m_pFrame->update();
}

void MainWindow::bgset_clicked()
{
    HBgProp bgsetProp(m_pFrame);
    bgsetProp.exec();
}

