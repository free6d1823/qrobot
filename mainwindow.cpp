#include <QDockWidget>

#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <QMessageBox>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <QIcon>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "comsetupdlg.h"

#include "controlpage.h"
#include "pagecali.h"
#include "pageservos.h"
#include "pageplay.h"
#include "pageseq.h"


static char mPort[64]="/dev/ttyUSB0";
static int mBaud = 115200;
static int mDatabits = 8;
static int mParity = 'n';
static int mStopbits = 1;
static bool mHfc = false;
static bool mSfc = false;

//thread
bool mStateRunning = false;
bool mTerminateThread = false;
pthread_t mThreadId = 0;

MainWindow* gMainWnd = NULL;
static SystemData sSystemData;
SystemData* gSystem = &sSystemData; //public his pointer

void OnUartReadCallback(const void* buffer, size_t length, void* pUserData)
{
    MainWindow* This = (MainWindow*)pUserData;
    This->postUpdateMessageEvent((const char* )buffer, length);
}


void MainWindow::resizeEvent(QResizeEvent *event)
{

}

int MainWindow::WriteMessage(const char* message, int length)
{
    return mUartCtrl.write(message, length);
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    gMainWnd = this;
    ui->setupUi(this);
    createMenuAndToolbar();

    loadSettings(DEFAULT_INI_NAME);
    createUi();
    //init tty

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::about()
{
   QMessageBox::about(this, tr("About QRobot"),
            tr("The <b>QRobot</b> demonstrates how to control a 6 DOF robot."));
}
void MainWindow::createMenuAndToolbar()
{
    QToolBar *fileToolBar = ui->mainToolBar;
    //// Menu FILE
    QMenu* fileMenu = ui->menuBar->addMenu(tr("&File"));
    const QIcon openIcon = QIcon(":/images/open.png");
    mOpenAct = fileMenu->addAction(openIcon, tr("&Open..."), this,
                    SLOT(onFileOpen()), QKeySequence::Open);
    mOpenAct->setStatusTip(tr("Open a project file."));
    const QIcon saveIcon = QIcon(":/images/save.png");
    mSaveAct = fileMenu->addAction(saveIcon, tr("&Save"), this,
                    SLOT(onFileSave()), QKeySequence::Save);
    mSaveAct->setStatusTip(tr("Save settings to project file."));
    fileMenu->addSeparator();

    const QIcon conIcon = QIcon(":/images/connect.png");
    mConAct = fileMenu->addAction(conIcon, tr("&Connect"), this,
                    SLOT(onFileConnect()), QKeySequence::Copy);
    mConAct->setStatusTip(tr("Disconnect COM port."));

    const QIcon disIcon = QIcon(":/images/disconnect.png");
    mDisAct = fileMenu->addAction(disIcon, tr("&Disconnect"), this,
                    SLOT(onFileConnect()), QKeySequence::Paste);
    mDisAct->setStatusTip(tr("Disconnect COM port."));

    fileMenu->addSeparator();
    const QIcon exitIcon =  QIcon(":/images/exit.png");
    QAction* exitAct = fileMenu->addAction(exitIcon,tr("E&xit"), this,
            SLOT(close()), QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit this program."));
    fileToolBar->addAction(mOpenAct);
    fileToolBar->addAction(mSaveAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(mConAct);
    fileToolBar->addAction(mDisAct);
    fileToolBar->addSeparator();

    //// Control /////////////////
    QMenu* controlMenu = ui->menuBar->addMenu(tr("&Control"));


    const char* item[MAX_PAGES] = {
        "Servo Control",
        "Calibration",
        "Sequence",
        "Editor"
    };
    const char* desc[MAX_PAGES] = {
        "Control each servo",
        "Servo calibration",
        "Sequencial play",
        "Show sequence editor"
    };

    mPageOff[0].addFile(":/images/page0_off.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOn[0].addFile(":/images/page0_on.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOff[1].addFile(":/images/page1_off.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOn[1].addFile(":/images/page1_on.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOff[2].addFile(":/images/page2_off.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOn[2].addFile(":/images/page2_on.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOff[3].addFile(":/images/page3_off.png",QSize(),QIcon::Normal, QIcon::On);
    mPageOn[3].addFile(":/images/page3_on.png",QSize(),QIcon::Normal, QIcon::On);
    mPageAct[0] = controlMenu->addAction(mPageOn[0], item[0], this, SLOT(onPage0()));
    mPageAct[1] = controlMenu->addAction(mPageOff[1], item[1], this, SLOT(onPage1()));
    mPageAct[2] = controlMenu->addAction(mPageOff[2], item[2], this, SLOT(onPage2()));
    mPageAct[3] = controlMenu->addAction(mPageOff[3], item[3], this, SLOT(onPage3()));
    for (int i=0; i< MAX_PAGES; i++){
        fileToolBar->addAction(mPageAct[i]);
        mPageAct[i]->setStatusTip(desc[i]);
        mPageAct[i]->setCheckable(true);
    }
    //// VIEW /////////////////
    QMenu* viewMenu = ui->menuBar->addMenu(tr("&View"));
    const QIcon zoominIcon = QIcon(":/images/zoomin.png");
    mZoominAct = viewMenu->addAction(zoominIcon, tr("&Zoom In"), this,
                    SLOT(onZoomin()), QKeySequence::ZoomIn);
    mZoominAct->setStatusTip(tr("Look near the robot"));
    fileToolBar->addAction(mZoominAct);
    const QIcon zoomoutIcon = QIcon(":/images/zoomout.png");
    mZoomoutAct = viewMenu->addAction(zoomoutIcon, tr("&Zoom Out"), this,
                    SLOT(onZoomout()), QKeySequence::ZoomOut);
    mZoominAct->setStatusTip(tr("Look far away the robot"));
    fileToolBar->addAction(mZoomoutAct);

    viewMenu->addSeparator();
    const QIcon clearIcon = QIcon(":/images/clear.png");
    mClearAct = viewMenu->addAction(clearIcon, tr("&Clear"), this,
                    SLOT(onViewClear()), QKeySequence::Delete);
    mClearAct->setStatusTip(tr("Clear the contents of the console."));
    fileToolBar->addSeparator();
    fileToolBar->addAction(mClearAct);
    // HELP  /////////////////////////
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, SLOT(about()), QKeySequence::HelpContents);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    QAction *helpAct = helpMenu->addAction(aboutIcon, tr("&SSC-32..."), this, SLOT(onCommandHelp()), QKeySequence::NextChild);
    helpAct->setStatusTip(tr("Show SSCC-32 command sets"));

    fileToolBar->addSeparator();
    fileToolBar->addAction(aboutAct);


    updateUi();

}
void MainWindow::createUi()
{
    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    mpConsoleView = new ConsleView(this);
    mpConsoleView->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    mpConsoleView->setAllowedAreas(Qt::BottomDockWidgetArea);
    mpConsoleView->setMinimumHeight(50);
    mpConsoleView->setSizePolicy(sizePolicy);
    addDockWidget(Qt::BottomDockWidgetArea, mpConsoleView);


    mpDockView = new QDockWidget(this);
    mpDockView->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    mpDockView->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

    mpDockView->setMinimumWidth(280);
    //mpDockView->setGeometry(0,0,300,600);

    mpDockView->setSizePolicy(sizePolicy);

    /* create all control pages */
    mpCtlPage[0] = new PageServos((QWidget*) mpDockView);
    mpCtlPage[1] = new PageCali((QWidget*) mpDockView);
    mpCtlPage[2] =  new PageSeq((QWidget*) mpDockView);
    mpCtlPage[3] =  new PagePlay((QWidget*) mpDockView);;

    mpCtlPage[0]->setVisible(false);
    mpCtlPage[1]->setVisible(false);
    mpCtlPage[2]->setVisible(false);
    mpCtlPage[3]->setVisible(false);
    addDockWidget(Qt::LeftDockWidgetArea, mpDockView);
    //must called after ini is loaded
    mCurrentPageId = -1;
    onPage0();

    mpMainView = new MainView(this);
    setCentralWidget(mpMainView);

    mpMainView->setVisible(true);


}
void MainWindow::updateUi()
{
    if(mUartCtrl.isOpened()) {
        mDisAct->setEnabled(true);
        mConAct->setEnabled(false);

    } else {
        mDisAct->setEnabled(false);
        mConAct->setEnabled(true);
    }
}
void MainWindow::onCommandHelp()
{
    QMessageBox::about(this, tr("SSC-32 Command Sets"),
             tr("<p><b>Servo Motion:</b>"
                "&nbsp;&nbsp;&nbsp;&nbsp;#&lt;ch&gt;P&lt;pw&gt;S&lt;spd&gt;T&lt;time&gt;&lt;cr&gt;<br>"

                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;ch&gt;&nbsp;&nbsp;&nbsp;&nbsp;PWM port, 0~31.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;pw&gt;&nbsp;&nbsp;&nbsp;&nbsp;PWM pules in us, 500~2500.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;spd&gt;&nbsp;&nbsp;&nbsp; Moving speed, in us/sec, optional.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;time&gt;&nbsp;&nbsp; Time to finish all actions, in ms. Must put in the last. Max. 65535, optional.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;cr&gt;&nbsp;&nbsp;&nbsp;&nbsp;Carret return code, 0x0D.<br>"
                "</p>"
                "<p><b>Level Output:</b>"
                "&nbsp;&nbsp;&nbsp;&nbsp;#&lt;ch&gt;&lt;lvl&gt;...#&lt;ch&gt;&lt;lvl&gt;&lt;cr&gt;<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;ch&gt;&nbsp;&nbsp;&nbsp;&nbsp;PWM port, 0~31.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;lvl&gt;Output voltage level. H for high, L for low.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;cr&gt; &#9; Carret return code, 0x0D.<br>"
                "</p>"
                "<p><b>Bitwise Level Output:</b>"
                "&nbsp;&nbsp;&nbsp;&nbsp;#&lt;bank&gt;:&lt;value&gt;&lt;cr&gt;<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;bank&gt;&nbsp;&nbsp;&nbsp;&nbsp;0~3. Bank 0 maps to channel 0~7, Bank 3 maps to channel 24~31.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;value&gt; 1 is high (5V), 0 is low (0V). Bit0 is LSB.<br>"
                "&nbsp;&nbsp;&nbsp;&nbsp;&lt;cr&gt; &#9; Carret return code, 0x0D.<br>"
                "</p>"
                ));
}
void MainWindow::onFileOpen()
{

}
void MainWindow::onFileSave()
{
    //update current page UI to Servo
    if(mCurrentPageId>=0 && mCurrentPageId < MAX_PAGES)
        mpCtlPage[mCurrentPageId]->saveUi();
    saveSettings(DEFAULT_INI_NAME);
}
void MainWindow::onViewClear()
{
    mpConsoleView->clear();
}

void MainWindow::onFileConnect()
{
    if(mUartCtrl.isOpened()) {
        mUartCtrl.close();
    } else {
        ComSetupDlg dlg;
        dlg.set(QString(mPort), mBaud, mDatabits, mParity, mStopbits, mHfc, mSfc);
        if( dlg.exec() == 1)
        {
            mBaud = dlg.mBaud;

            strncpy(mPort, dlg.mPort.toLocal8Bit().constData(), sizeof(mPort));
            mDatabits = dlg.mDatabits;
            mParity = dlg.mParity;
            mStopbits = dlg.mStopbits;
            mHfc= dlg.mHfc;
            mSfc = dlg.mSfc;
            mUartCtrl.setBaud(mBaud);
            mUartCtrl.setAttrib(mDatabits,mStopbits,mParity,mHfc,mSfc);
            if (0 > mUartCtrl.open(mPort, OnUartReadCallback, this))
            {
                QMessageBox::critical(this,tr("Open Com"),tr("Open UART failed!"));
            }else {
                WriteMessage("VER\n", 4);
                WriteMessage("#0P1200T1\n", 10);
            }

        }
    }
    updateUi();
}

void MainWindow::customEvent(QEvent* event)
{
    if(event->type() == MY_CUSTOM_EVENT)
    {
        handleUpdateMessageEvent(static_cast<UpdateMEssageEvent *>(event));
    }
}

void MainWindow::postUpdateMessageEvent(const char* message, int length)
{
static char szText[1024];

    if (length > sizeof(szText)-1)
        length = sizeof(szText)-1;
    memcpy(szText, message, length);
    //make text visible
    szText[length] = 0;
    QApplication::postEvent(this,
                            new UpdateMEssageEvent(szText, length));

}
//handle the message in GUI thread
void MainWindow::handleUpdateMessageEvent(UpdateMEssageEvent *event)
{
    mpConsoleView->appendMessage(event->getMessage() );
}
//must call from UI thread
void MainWindow::appendLog(char* log)
{
    mpConsoleView->appendMessage(log );
}

void MainWindow::onPage0(){doEnablePage(0);}
void MainWindow::onPage1(){doEnablePage(1);}
void MainWindow::onPage2(){doEnablePage(2);}
void MainWindow::onPage3(){doEnablePage(3);}
void MainWindow::doEnablePage(int id)
{
    for (int i=0; i< MAX_PAGES; i++){
        //mPageAct[i]->setChecked(i!=id);
        mPageAct[i]->setChecked(i==id);
    }
    //save previous page, mCurrentPageId=-1 if first time
    if(mCurrentPageId>=0 && mCurrentPageId < MAX_PAGES && mpCtlPage[mCurrentPageId]){
        mpCtlPage[mCurrentPageId]->saveUi();
        mpCtlPage[mCurrentPageId]->activate(false);
        mPageAct[mCurrentPageId]->setIcon(mPageOff[mCurrentPageId]);
    }
    mCurrentPageId = id;
    if(mpCtlPage[mCurrentPageId]) {
        mpCtlPage[mCurrentPageId]->activate(true);
        mPageAct[mCurrentPageId]->setIcon(mPageOn[mCurrentPageId]);
    }
}


bool MainWindow::loadSettings(const char* szIniName)
{
    return gSystem->loadIni(szIniName);
}
bool MainWindow::saveSettings(const char* szIniName)
{
    return gSystem->saveIni(szIniName);

}
