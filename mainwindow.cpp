#include <QDockWidget>
#include <termios.h>
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

static int mFd = -1; //handle to UART
static char mPort[64]="/dev/ttyUSB0";
static int mBaud = B115200;
static bool mEightbits = true;
static bool mParity = false;
static bool mOneStop = true;
static bool mHfc = false;
static bool mSfc = false;

//thread
bool mStateRunning = false;
bool mTerminateThread = false;
pthread_t mThreadId = 0;

MainWindow* gMainWnd = NULL;

void* ReadThread(void* data)
{
    MainWindow* This = (MainWindow*)data;
    mStateRunning = true;
    mTerminateThread = false;

    unsigned char rb[1024];
    while(!mTerminateThread) {
        if (mFd < 0)
            break;
        int c = read(mFd, &rb, sizeof(rb));
        if (c > 0) {
            This->WriteMessage((const char*)rb, c);
            printf("read %d\n", c);
        }
        usleep(10000);
    }
    mStateRunning = false;
    return (void*)0;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QRect rc = mpDockView2->geometry();
    mpConsole->setGeometry(rc); ;
}

int MainWindow::WriteMessage(const char* message, int length)
{
    if (mFd < 0)
        return -1;
    return (int) write(mFd, message, length);
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
    closeCom();
    delete ui;
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
}
void MainWindow::createUi()
{

    mpDockView2 = new QDockWidget(this);
    mpDockView2->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    mpDockView2->setAllowedAreas(Qt::BottomDockWidgetArea);
    mpConsole = new Console((QWidget*) mpDockView2);
    addDockWidget(Qt::BottomDockWidgetArea, mpDockView2);

    QRect rc = mpDockView2->geometry();

    QSizePolicy sizePolicy;
    sizePolicy.setHorizontalPolicy(QSizePolicy::Expanding);
    sizePolicy.setVerticalPolicy(QSizePolicy::Expanding);
    mpConsole->setSizePolicy(sizePolicy);
    mpConsole->setGeometry(rc); ;


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
    if(mFd>=0) {
        mCloseAct->setEnabled(true);
        mOpenAct->setEnabled(false);

    } else {
        mCloseAct->setEnabled(false);
        mOpenAct->setEnabled(true);
    }
}
void MainWindow::onFileClear()
{
    mpConsole->appendMessage("Hello");
}

void MainWindow::onFileOpen()
{
    if(mFd >=0) {
        closeCom();
        mFd = -1;
    } else {
        ComSetupDlg dlg;
        dlg.updateUi(QString(mPort), mBaud, mEightbits, mParity, mOneStop, mHfc, mSfc);
        if( dlg.exec() == 1)
        {
            mBaud = dlg.mBaud;

            strncpy(mPort, dlg.mPort.toLocal8Bit().constData(), sizeof(mPort));
            mEightbits = dlg.mEightbits;
            mParity = dlg.mPar;
            mOneStop = dlg.mOne;
            mHfc= dlg.mHfc;
            mSfc = dlg.mSfc;
            openCom();

        }
    }
    updateUi();
}

void MainWindow::closeCom()
{
    if (mStateRunning) {
        mTerminateThread = true;
        pthread_join(mThreadId, NULL);
        mStateRunning = false; //force false
        mThreadId = 0;
    }
    if (mFd >=0 ){
        ::close(mFd);
        mFd = -1;
    }
}

int MainWindow::openCom()
{
    mFd = -1;
    //check port
    struct stat   sb;
    if ( stat(mPort, &sb) != 0) {
        QMessageBox mess(QMessageBox::Warning, "open", "Invalide UART path!", QMessageBox::Close, this);
        mess.exec();
        return -1;
    }

    int fd = ::open (mPort, O_RDWR | O_NOCTTY | O_DSYNC, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        char buff[256];
        sprintf(buff, "Failed to open device %s.\n%s.", mPort,
                strerror(errno));
        QMessageBox mess(QMessageBox::Warning, "Open", buff, QMessageBox::Close, this);
        mess.exec();
        return fd;
    }
    //////////////////////////////
    /// \brief configure TTY
    ///
    while (fd >= 0) {
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
            char buff[256];
            sprintf(buff, "Error from tcgetattr.\n%s.", strerror(errno));
            QMessageBox mess(QMessageBox::Warning, "Open", buff, QMessageBox::Close, this);
            mess.exec();
            break;

        }

        cfsetospeed (&tty, mBaud);
        cfsetispeed (&tty, mBaud);
        if (mEightbits)
            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        else
            tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS7;     // 7-bit chars

         // disable IGNBRK for mismatched speed tests; otherwise receive break
         // as \000 chars
         tty.c_iflag &= ~IGNBRK;         // disable break processing
         tty.c_lflag = 0;                // no signaling chars, no echo,
                                         // no canonical processing
         tty.c_oflag = 0;                // no remapping, no delays
         tty.c_cc[VMIN]  = 0;            // read doesn't block
         tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

         if (mSfc)
             tty.c_iflag |= (IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
         else
             tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

         tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                         // enable reading
         if (!mParity) {
            tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
         }
         else {
            tty.c_cflag |= (PARENB);//even parity for simple // | PARODD);
            tty.c_cflag |= PARODD;    //是odd同位元檢查
         }
         if (mOneStop) {
             tty.c_cflag |= ~CSTOPB; //not two stops
         } else {
              tty.c_cflag &= CSTOPB; //two stops
         }
         if (mHfc)
             tty.c_cflag |= CRTSCTS;
         else
             tty.c_cflag &= ~CRTSCTS;

         /* now clean the modem line and activate the settings for the port */
        tcflush(fd, TCIOFLUSH);

         if (tcsetattr (fd, TCSANOW, &tty) != 0)
         {
             char buff[256];
             sprintf(buff, "Error from tcsetattr.\n%s.", strerror(errno));
             QMessageBox mess(QMessageBox::Warning, "Open", buff, QMessageBox::Close, this);
             mess.exec();
             break;
         }
        mFd = fd;

        ////create read thread
        ///
        ///
        pthread_create(&mThreadId, NULL, ReadThread, (void*) this);
        break;
    }
    if (mFd != fd) {
       ::close(fd);
    }
    return mFd;
}
