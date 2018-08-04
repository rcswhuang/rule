#if defined(_MSC_VER) &&(_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include "hrulewindow.h"
#include "ui_rulewindow.h"
#include <QVBoxLayout>
#include <QHeaderView>
#include <QScrollBar>
#include <QToolBar>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QMessageBox>
#include "hdrawtool.h"
//#include "hbgprop.h"
#include "hviewprop.h"
//#include "hsimulateprop.h"
#include "hrulefile.h"
#include "hruledoc.h"
extern LPRULEDATACALLBACK m_lpRuleDataCallBack;
extern quint8 m_btAppType;

HRuleWindow::HRuleWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HRuleWindow)
{
    ui->setupUi(this);
    createActions();
    createToolBar();
    //createDockWindows();

    m_pScrollArea = new QScrollArea(this);
    m_pFrame = new HFrame(m_pScrollArea);
    m_pScrollArea->setWidget(m_pFrame);
    if(m_pFrame && m_pFrame->pRuleFile)
        m_pScrollArea->widget()->resize(m_pFrame->pRuleFile->m_Size.width(),m_pFrame->pRuleFile->m_Size.height());
    else
        m_pScrollArea->widget()->resize(1200,1000);
    ui->verticalLayout_2->addWidget(m_pScrollArea);
    resize(800,600);
}

HRuleWindow::HRuleWindow(HRuleFile* rf,QWidget *parent)
    :m_pRuleFile(rf),QDialog(parent),
      ui(new Ui::HRuleWindow)
{
      ui->setupUi(this);
      createActions();
      createToolBar();
      setWindowFlags(Qt::Dialog);
      setWindowModality(Qt::ApplicationModal);
      m_pScrollArea = new QScrollArea(this);
      m_pFrame = new HFrame(m_pScrollArea);
      m_pFrame->setRuleFile(m_pRuleFile);
      m_pScrollArea->setWidget(m_pFrame);
      if(m_pFrame && m_pFrame->pRuleFile)
          m_pScrollArea->widget()->resize(m_pFrame->pRuleFile->m_Size.width(),m_pFrame->pRuleFile->m_Size.height());
      else
          m_pScrollArea->widget()->resize(1200,1000);
      ui->verticalLayout_2->addWidget(m_pScrollArea);
      setWindowTitle(rf->m_strRuleName);
      resize(800,600);
}


HRuleWindow::~HRuleWindow()
{
    delete ui;
}

void HRuleWindow::setRuleDoc(HRuleDoc *doc)
{
    m_pRuleDoc = doc;
}

void HRuleWindow::createActions()
{
    //setIconSize(QSize(16,16));
    okAct = new QAction(QIcon(":image/ok.png"),tr("确定"),this);
    connect(okAct,SIGNAL(triggered(bool)),this,SLOT(okBtn_clicked()));
    cancelAct = new QAction(QIcon(":image/cancel.png"),tr("取消"),this);
    connect(cancelAct,SIGNAL(triggered(bool)),this,SLOT(close()));
    delAct = new QAction(QIcon(":image/edit_del.png"),tr("删除"),this);
    connect(delAct,SIGNAL(triggered(bool)),this,SLOT(del_clicked()));
    cutAct = new QAction(QIcon(":image/edit_cut.png"),tr("剪切"),this);
    cutAct->setShortcut(QKeySequence::Cut);
    connect(cutAct,SIGNAL(triggered(bool)),this,SLOT(cut_clicked()));
    copyAct = new QAction(QIcon(":image/edit_copy.png"),tr("拷贝"),this);
    copyAct->setShortcut(QKeySequence::Copy);
    connect(copyAct,SIGNAL(triggered(bool)),this,SLOT(copy_clicked()));
    pasteAct = new QAction(QIcon(":image/edit_paste.png"),tr("&粘贴"),this);
    pasteAct->setShortcut(QKeySequence::Paste);
    connect(pasteAct,SIGNAL(triggered(bool)),this,SLOT(paste_clicked()));

    formulaAct = new QAction(QIcon(":image/formula.png"),tr("生成公式"),this);
    attrAct = new QAction(QIcon(":image/report.png"),tr("规则报告"),this);
    idAct = new QAction(QIcon(":image/ID.png"),tr("ID"),this);
    connect(idAct,SIGNAL(triggered(bool)),this,SLOT(idset_clicked()));
    simuAct = new QAction(QIcon(":image/simulation.png"),tr("公式仿真"),this);
    connect(simuAct,SIGNAL(triggered(bool)),this,SLOT(simulate_clicked()));

    fullAct = new QAction(QIcon(":image/size.png"),tr("调整大小"),this);
    connect(fullAct,SIGNAL(triggered(bool)),this,SLOT(sizeset_clicked()));
    bgAct = new QAction(QIcon(":image/bgcolor.png"),tr("颜色设置"),this);
    connect(bgAct,SIGNAL(triggered(bool)),this,SLOT(bgset_clicked()));
    gridAct = new QAction(QIcon(":image/grid.png"),tr("显示网格"),this);
    connect(gridAct,SIGNAL(triggered(bool)),this,SLOT(gridset_clicked()));
    zoominAct = new QAction(QIcon(":image/zoom_in.png"),tr("放大"),this);
    connect(zoominAct,SIGNAL(triggered(bool)),this,SLOT(zoomin_clicked()));
    zoomoutAct = new QAction(QIcon(":image/zoom_out.png"),tr("缩小"),this);
    connect(zoomoutAct,SIGNAL(triggered(bool)),this,SLOT(zoomout_clicked()));


    digitalPutAct = new QAction(QIcon(":/image/digital.png"),tr("&遥信"),this);
    connect(digitalPutAct,SIGNAL(triggered()),this,SLOT(onCreateDigitalInput()));
    analogueAct = new QAction(QIcon(":image/analogue.png"),tr("遥测"),this);
    connect(analogueAct,SIGNAL(triggered()),this,SLOT(onCreateAnalogueInput()));
    selectAct = new QAction(QIcon(":image/select.png"),tr("&Select"),this);
    connect(selectAct,SIGNAL(triggered(bool)),this,SLOT(select_clicked()));
    textAct = new QAction(QIcon(":image/text.png"),tr("&Delete"),this);
    lineAct = new QAction(QIcon(":image/line.png"),tr("&线条"),this);
    connect(lineAct,SIGNAL(triggered()),this,SLOT(onCreateConnnectLine()));

    andAct = new QAction(QIcon(":image/and.png"),tr("与"),this);
    connect(andAct,SIGNAL(triggered()),this,SLOT(onCreateLogicAnd()));
    orAct = new QAction(QIcon(":image/or.png"),tr("或"),this);
    connect(orAct,SIGNAL(triggered()),this,SLOT(onCreateLogicOr()));
    outAct = new QAction(QIcon(":image/out.png"),tr("输出"),this);
    connect(outAct,SIGNAL(triggered()),this,SLOT(onCreateResultOutput()));
}

void HRuleWindow::createDockWindows()
{
 /*   rightDock = new QDockWidget(this);
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
    */
    //addDockWidget(Qt::RightDockWidgetArea,rightDock);

}

void HRuleWindow::createToolBar()
{

    confirmToolBar = new QToolBar;
    confirmToolBar->setOrientation(Qt::Horizontal);
    confirmToolBar->setIconSize(QSize(24,24));
    confirmToolBar->setMovable(false);
    confirmToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    confirmToolBar->addAction(cancelAct);
    confirmToolBar->addAction(okAct);
    //addToolBar(Qt::TopToolBarArea,confirmToolBar);
    //addToolBarBreak();
    //int width1 = width()-100;
    //ui->horizontalLayout->insertSpacing(0,width1);
    ui->horizontalLayout->addWidget(confirmToolBar,5,Qt::AlignRight);

    /*editToolBar = new QToolBar;
    editToolBar->setIconSize(QSize(20,20));
    editToolBar->setMovable(false);
    editToolBar->addAction(delAct);
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
    */
    //addToolBar(editToolBar);
    //ui->horizontalLayout_3->addWidget(editToolBar);

    //只能放到一个toolBar里面
    attrToolBar = new QToolBar;
    attrToolBar->setIconSize(QSize(24,24));
    attrToolBar->setMovable(false);

    attrToolBar->setMovable(false);
    attrToolBar->addAction(delAct);
    attrToolBar->addAction(cutAct);
    attrToolBar->addAction(copyAct);
    attrToolBar->addAction(pasteAct);
    attrToolBar->addSeparator();

    //attrToolBar->addAction(formulaAct);
    //attrToolBar->addAction(attrAct);
    //attrToolBar->addAction(idAct);
    //attrToolBar->addAction(simuAct);
    //addToolBar(attrToolBar);
    //ui->horizontalLayout_3->addWidget(attrToolBar);

    //configToolBar = new QToolBar;
    //configToolBar->setIconSize(QSize(20,20));
    //configToolBar->setMovable(false);
    attrToolBar->addAction(fullAct);
    //attrToolBar->addAction(bgAct);
    attrToolBar->addAction(gridAct);
    //attrToolBar->addAction(zoominAct);
    //attrToolBar->addAction(zoomoutAct);
    //addToolBar(configToolBar);
    attrToolBar->addSeparator();


    //logicToolBar = new QToolBar;
    //logicToolBar->setOrientation(Qt::Horizontal);
    //logicToolBar->setIconSize(QSize(24,24));
    //logicToolBar->setMovable(false);
    //logicToolBar->addAction(selectAct);
    //logicToolBar->addAction(lineAct);
    //logicToolBar->addAction(textAct);
    attrToolBar->addAction(selectAct);
    attrToolBar->addAction(andAct);
    attrToolBar->addAction(orAct);
    attrToolBar->addAction(digitalPutAct);
    attrToolBar->addAction(analogueAct);
    attrToolBar->addAction(outAct);
    //addToolBar(Qt::LeftToolBarArea,logicToolBar);
    //ui->verticalLayout->addWidget(logicToolBar);
    ui->horizontalLayout_3->addWidget(attrToolBar);
}

void HRuleWindow::onCreateDigitalInput()
{
    //m_pFrame->setDrawShape(enumInputPOINT);
    HDrawTool::drawShape = enumInputDIGITAL;
}

void HRuleWindow::onCreateAnalogueInput()
{
    HDrawTool::drawShape = enumInputANALOGUE;
}

void HRuleWindow::onCreateConnnectLine()
{
    HDrawTool::drawShape = enumConnect;
}

void HRuleWindow::onCreateResultOutput()
{
    HDrawTool::drawShape = enumOutRESUTL;
}

void HRuleWindow::onCreateLogicOr()
{
    HDrawTool::drawShape = enumLogicOR;
}

void HRuleWindow::onCreateLogicAnd()
{
    HDrawTool::drawShape = enumLogicAND;
}

void HRuleWindow::select_clicked()
{
    HDrawTool::drawShape = enumSelection;
}

void HRuleWindow::okBtn_clicked()
{
    if(maybeSave())
        QDialog::accept();
    else
        QDialog::reject();
}

bool HRuleWindow::save()
{
    if(!m_pRuleDoc || !m_pRuleDoc->m_pRuleFile)
        return false;
    if(!m_pRuleFile->buildGeneralFormula() && !m_pRuleFile->m_drawObjList.isEmpty())
    {
        if(QMessageBox::Yes == QMessageBox::warning(this, QStringLiteral("警告"),QStringLiteral("规则中存在错误，需要继续编辑吗?"),QMessageBox::Yes | QMessageBox::No))
        {
            return false;
        }
        m_pRuleFile->m_strFormula = "";
        return true;
    }
    //m_pRuleFile指向的是m_pTempRuleFile
    m_pRuleFile->copyTo(m_pRuleDoc->m_pRuleFile);
    return true;
}

bool HRuleWindow::maybeSave()
{
  if (!m_pRuleDoc)
      return false;
  if(!m_pRuleDoc->m_bModify) //没有修改
      return true;
  const QMessageBox::StandardButton ret
      = QMessageBox::warning(this, QStringLiteral("警告"),
                             QStringLiteral("规则文件已经修改，需要保存吗?"),
                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
  case QMessageBox::Save:
      return save();
  case QMessageBox::Cancel:
      return false;
  default:
      break;
  }
  return true;
}

void HRuleWindow::closeEvent(QCloseEvent *e)
{
  if (maybeSave()) {
      e->accept();
  } else {
      e->ignore();
  }

}
/*
void MainWindow::onDrawGrid()
{
    m_pFrame->m_bGrid = true;
    m_pFrame->update();
}*/

void HRuleWindow::bgset_clicked()
{
    //HBgProp bgsetProp(m_pFrame);
    //bgsetProp.exec();
}

void HRuleWindow::gridset_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->pRuleFile->m_bGrid = !m_pFrame->pRuleFile->m_bGrid;
    m_pFrame->update();
}

void HRuleWindow::idset_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    //m_pFrame->pRuleFile->bDisplayID = !m_pFrame->pRuleFile->bDisplayID;
    //m_pFrame->update();
}

void HRuleWindow::sizeset_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    HViewProp viewProp(m_pFrame->pRuleFile);
    viewProp.exec();
    m_pFrame->resize(m_pFrame->pRuleFile->m_Size.width(),m_pFrame->pRuleFile->m_Size.height());
}

void HRuleWindow::zoomin_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->factor += (float)0.1;
    if(m_pFrame->factor < (float)0.5) m_pFrame->factor = (float)0.5;
    if(m_pFrame->factor > (float)1.5) m_pFrame->factor = (float)1.5;
    m_pFrame->resize(m_pFrame->pRuleFile->m_Size.width()*m_pFrame->factor,m_pFrame->pRuleFile->m_Size.height()*m_pFrame->factor);
}

void HRuleWindow::zoomout_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->factor -= (float)0.1;
    if(m_pFrame->factor < (float)0.5) m_pFrame->factor = (float)0.5;
    if(m_pFrame->factor > (float)1.5) m_pFrame->factor = (float)1.5;
    m_pFrame->resize(m_pFrame->pRuleFile->m_Size.width()*m_pFrame->factor,m_pFrame->pRuleFile->m_Size.height()*m_pFrame->factor);
}

void HRuleWindow:: del_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->delObj();
}

void HRuleWindow:: cut_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->cutObj();
}

void HRuleWindow:: copy_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->copyObj();
}

void HRuleWindow:: paste_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
    m_pFrame->pasteObj();
}

void HRuleWindow::simulate_clicked()
{
    if(!m_pFrame && !m_pFrame->pRuleFile)
        return;
   /* if(!m_pFrame->pRuleFile->bSimuState)
    {
        HSimulateProp prop(m_pFrame->pRuleFile);
        prop.exec();
        //需要遍历一遍
        m_pFrame->pRuleFile->buildSimulateFormula();
    }
    m_pFrame->pRuleFile->bSimuState = !m_pFrame->pRuleFile->bSimuState;
    m_pFrame->update();*/
}
