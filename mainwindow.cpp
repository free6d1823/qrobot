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
#include "controlpanel.h"
#include "comsetupdlg.h"
#include "uartctrl.h"


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
static UartCtrl mUartCtrl;
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
    ui->setupUi(this);
    createMenuAndToolbar();
    createUi();
    //init tty
    gMainWnd = this;
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
    mOpenAct = fileMenu->addAction(openIcon, tr("&Open..."), this,
                    SLOT(onFileOpen()), QKeySequence::Open);
    const QIcon closeIcon = QIcon(":/images/save.png");
    mCloseAct = fileMenu->addAction(closeIcon, tr("&Close"), this,
                    SLOT(onFileOpen()), QKeySequence::Close);

    fileMenu->addSeparator();
    const QIcon clearIcon = QIcon(":/images/ruler.png");
    mClearAct = fileMenu->addAction(clearIcon, tr("&Clear"), this,
                    SLOT(onFileClear()), QKeySequence::Delete);

    updateUi();
    fileToolBar->addAction(mOpenAct);
    fileToolBar->addAction(mCloseAct);
    fileToolBar->addAction(mClearAct);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    const QIcon aboutIcon = QIcon(":/images/about.png");
    QAction *aboutAct = helpMenu->addAction(aboutIcon, tr("&About"), this, SLOT(about()), QKeySequence::HelpContents);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    fileToolBar->addAction(aboutAct);



}
void MainWindow::createUi()
{

    mpConsoleView = new ConsleView(this);
    mpConsoleView->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    mpConsoleView->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, mpConsoleView);


    mpDockView = new QDockWidget(this);
    mpDockView->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    mpDockView->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    mpControlPanel = new ControlPanel((QWidget*) mpDockView);
    addDockWidget(Qt::LeftDockWidgetArea, mpDockView);

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
