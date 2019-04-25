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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "comsetupdlg.h"

#include "calibrate.h"
#include "servoctrl.h"
#include "inifile/inifile.h"
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
    QMenu* fileMenu = ui->menuBar->addMenu(tr("&File"));
    const QIcon openIcon = QIcon(":/images/open.png");
    mOpenAct = fileMenu->addAction(openIcon, tr("&Connect..."), this,
                    SLOT(onFileOpen()), QKeySequence::Open);
    mOpenAct->setStatusTip(tr("Open a COM port."));

    const QIcon closeIcon = QIcon(":/images/close.png");
    mCloseAct = fileMenu->addAction(closeIcon, tr("&Disconnect"), this,
                    SLOT(onFileOpen()), QKeySequence::Close);
    mCloseAct->setStatusTip(tr("Disconnect COM port."));

    const QIcon saveIcon = QIcon(":/images/save.png");
    QAction* saveAct = fileMenu->addAction(saveIcon, tr("&Save"), this,
                    SLOT(onFileSave()), QKeySequence::Save);
    saveAct->setStatusTip(tr("Save settings."));

    fileMenu->addSeparator();
    const QIcon exitIcon =  QIcon(":/images/exit.png");
    QAction* exitAct = fileMenu->addAction(exitIcon,tr("E&xit"), this,
            SLOT(close()), QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit this program."));
    fileToolBar->addAction(mOpenAct);
    fileToolBar->addAction(mCloseAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addSeparator();
    /////////////////////
    QMenu* viewMenu = ui->menuBar->addMenu(tr("&View"));


    const QIcon p0Icon = QIcon(":/images/control.png");
    mPageAct[0] = viewMenu->addAction(p0Icon, tr("C&ontrol"), this,
                    SLOT(onPage0()));
    mPageAct[0]->setStatusTip(tr("Control each servo."));

    const QIcon p1Icon = QIcon(":/images/calibrate.png");
    mPageAct[1] = viewMenu->addAction(p1Icon, tr("C&alibration"), this,
                    SLOT(onPage1()));
    mPageAct[1]->setStatusTip(tr("Servo port setting and calibration."));

    const QIcon p2Icon = QIcon(":/images/sequence.png");
    mPageAct[2] = viewMenu->addAction(p2Icon, tr("&Sequence"), this,
                    SLOT(onPage2()));
    mPageAct[2]->setStatusTip(tr("Sequence play."));
    const QIcon p3Icon = QIcon(":/images/seqedit.png");
    mPageAct[3] = viewMenu->addAction(p3Icon, tr("Sequence &Edit"), this,
                    SLOT(onPage3()));
    mPageAct[3]->setStatusTip(tr("Sequence editor."));
    viewMenu->addSeparator();
    const QIcon clearIcon = QIcon(":/images/clear.png");
    mClearAct = viewMenu->addAction(clearIcon, tr("&Clear"), this,
                    SLOT(onFileClear()), QKeySequence::Delete);
    mClearAct->setStatusTip(tr("Clear the contents of the console."));


    for (int i=0; i< MAX_PAGES; i++)
        fileToolBar->addAction(mPageAct[i]);
    fileToolBar->addSeparator();
    fileToolBar->addAction(mClearAct);


    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, SLOT(about()), QKeySequence::HelpContents);
    aboutAct->setStatusTip(tr("Show the application's About box"));
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


    mpServoPage = new ServoPage((QWidget*) mpDockView);
    mpCaliPage = new CaliPage((QWidget*) mpDockView);
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
        mCloseAct->setEnabled(true);
        mOpenAct->setEnabled(false);

    } else {
        mCloseAct->setEnabled(false);
        mOpenAct->setEnabled(true);
    }
}
void MainWindow::onFileClear()
{
    mpConsoleView->clear();
}

void MainWindow::onFileOpen()
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
void MainWindow::doEnablePage(int id)
{
    for (int i=0; i< MAX_PAGES; i++){
        mPageAct[i]->setEnabled(i!=id);
        mPageAct[i]->setChecked(i==id);
    }
    //save previous page
     switch(mCurrentPageId){
    case 0: mpServoPage->saveUi();break;
    case 1:    mpCaliPage->saveUi();break;
    default:
        break;
    }

}
void MainWindow::onPage0()
{
    doEnablePage(0);

    mpCaliPage->activate(false); //save previous page
    mpServoPage->activate(true);

    //page view changed to new ID
    mCurrentPageId = 0;
}
void MainWindow::onPage1()
{
    doEnablePage(1);

    mpServoPage->activate(false);
    mpCaliPage->activate(true);
    //page view changed to new ID
    mCurrentPageId = 1;
}
void MainWindow::onPage2()
{
    doEnablePage(2);
    //page view changed to new ID
    mCurrentPageId = 2;
}
void MainWindow::onPage3()
{
    doEnablePage(3);
    //page view changed to new ID
    mCurrentPageId = 3;
}
void MainWindow::onFileSave()
{
    //update current page UI to Servo
    switch(mCurrentPageId){
   case 0: mpServoPage->saveUi();break;
   case 1:    mpCaliPage->saveUi();break;
   default:
       break;
   }
    saveSettings(DEFAULT_INI_NAME);
}
bool MainWindow::loadSettings(const char* szIniName)
{
    void* handle  = openIniFile(szIniName, true);
    if(!handle)
        return false;
    int nMAXServo =
        GetProfileInt("system", "servoCounts", MAX_SERVOS_NUMBER, handle);
    if (nMAXServo > MAX_SERVOS_NUMBER)
        nMAXServo = MAX_SERVOS_NUMBER;
    char session[64];
    char szValue[256];
    int nValue;
    float fValue;
    for (int i=0; i< nMAXServo; i++) {
        sprintf(session, "servo_%d", i);
        nValue = GetProfileInt(session, "port", i, handle);
        GetProfileString(session, "name", szValue, sizeof(szValue), "", handle );
        mServo[i].setPortName(szValue, nValue);
        fValue = GetProfileFloat(session, "minAngle", MIN_ANGLE, handle);
        mServo[i].setMinAngle(fValue);
        fValue = GetProfileFloat(session, "maxAngle", MAX_ANGLE, handle);
        mServo[i].setMaxAngle(fValue);
        fValue = GetProfileFloat(session, "centerAngle", CENTER_ANGLE, handle);
        mServo[i].setCenterAngle(fValue);
        fValue = GetProfileFloat(session, "centerPw", DEFAULT_CENTER_PW, handle);
        mServo[i].setCenterPw(fValue);
        fValue = GetProfileFloat(session, "minPw", DEFAULT_MIN_PW, handle);
        mServo[i].setMinPw(fValue);
        fValue = GetProfileFloat(session, "maxPw", DEFAULT_MAX_PW, handle);
        mServo[i].setMaxPw(fValue);
        mServo[i].calibration();
    }

    closeIniFile(handle);

    return true;
}
bool MainWindow::saveSettings(const char* szIniName)
{
    void* handle  = openIniFile(szIniName, false);
    if(!handle)
        return false;
    WriteProfileInt("system", "servoCounts", MAX_SERVOS_NUMBER, handle);

    char session[64];

    int nValue;

    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        sprintf(session, "servo_%d", i);
        char* name = mServo[i].getPortName(&nValue);
        WriteProfileInt(session, "port", nValue, handle);
        WriteProfileString(session, "name", name, handle );

        WriteProfileFloat(session, "minAngle",  mServo[i].minAngle(), handle);
        WriteProfileFloat(session, "maxAngle",  mServo[i].maxAngle(), handle);
        WriteProfileFloat(session, "centerAngle",  mServo[i].centerAngle(), handle);
        WriteProfileFloat(session, "minPw",  mServo[i].minPw(), handle);
        WriteProfileFloat(session, "maxPw",  mServo[i].maxPw(), handle);
        WriteProfileFloat(session, "centerPw",  mServo[i].centerPw(), handle);
    }
    closeIniFile(handle);

    return true;
}
