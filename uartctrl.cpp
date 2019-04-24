#include <stdio.h>      /*標準輸入輸出定義*/
#include <stdlib.h>     /*標準函數庫定義*/
#include <unistd.h>     /*Unix標準函數定義*/
#include <sys/types.h>  /**/
#include <sys/stat.h>   /**/
#include <fcntl.h>      /*檔控制定義*/
#include <termios.h>    /*PPSIX終端控制定義*/
#include <errno.h>      /*錯誤號定義*/
#include <pthread.h>
#include "uartctrl.h"
#include <QMessageBox>

/***@brief  設置串口通信速率
*@param  fd     類型 int  打開串口的文件控制碼
*@param  speed  類型 int  串口速度
*@return  void
*/

static int speed_arr[] = { B230400, B115200, B57600, B19200, B9600, B4800 };
static int name_arr[] =  {230400, 115200, 57600,  19200,  9600,  4800};

static bool setBaudRate(int fd, int speed)

{
    size_t   i;
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);

    for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {

        if  (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0) {
                perror("tcsetattr fd1");
                return false;
            }
        }
        tcflush(fd,TCIOFLUSH);
   }
    return true;
}

/**
*@brief   設置串口資料位元，停止位元和效驗位
*@param  fd     類型  int  打開的串口文件控制碼*
*@param  databits 類型  int 資料位元   取值 為 7 或者8*
*@param  stopbits 類型  int 停止位   取值為 1 或者2*
*@param  parity  類型  int  效驗類型 取值為N,E,O,,S
*@param  bHfc enable hardware flow control
*#param  bSfc enable software flow control
*
*/

static int setParity(int fd,int databits,int stopbits,int parity, bool bHfc, bool bSfc)
{
    if (fd < 0) {
        perror("Invalide file descriptor!\n");
        return -1;
    }
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0)
    {
        perror("SetupSerial 1");
        return -1;
    }
    if(!bSfc)
        options.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl
    else
        options.c_iflag |= (IXON | IXOFF | IXANY); // xon/xoff ctrl
    if(!bHfc)
        options.c_cflag &= ~CRTSCTS;
    else
        options.c_cflag |= CRTSCTS;

    options.c_cflag &= ~CSIZE;
    switch (databits) /*設置數據位元數*/
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unsupported data size\n");
            return -2;
    }

    switch (parity)
    {
    case 'n':
    case 'N':
        options.c_cflag &= ~PARENB;   /* Clear parity enable */
        options.c_iflag &= ~INPCK;     /* Enable parity checking */
        break;

    case 'o':
    case 'O':
        options.c_cflag |= (PARODD | PARENB);  /* 設置為奇效驗*/
        options.c_iflag |= INPCK;             /* Disnable parity checking */
        break;
    case 'e':
    case 'E':
        options.c_cflag |= PARENB;     /* Enable parity */
        options.c_cflag &= ~PARODD;   /* 轉換為偶效驗*/
        options.c_iflag |= INPCK;       /* Disnable parity checking */
        break;
    case 'S':
    case 's':  /*as no parity*/
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        break;

    default:
        fprintf(stderr,"Unsupported parity\n");
        return -3;
    }

    /* 設置停止位*/
    switch (stopbits)
    {
    case 1:
        options.c_cflag &= ~CSTOPB;
        break;
    case 2:
        options.c_cflag |= CSTOPB;
         break;
    default:
         fprintf(stderr,"Unsupported stop bits\n");
         return (-4);
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;
    tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */

    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 5");
        return (-5);
    }
    return (0);
}


UartCtrl::UartCtrl():mTerminateThread(false), mFd(-1),  mfnReadCallback(nullptr), mpUserData(nullptr),
    mpReceiveBuffer(nullptr)
{
    mBaudRate = 115200;
    mDatabits = 8;
    mStopbits = 1;
    mParity = 'n';
    mHfc = false;
    mSfc = false;
}

UartCtrl::~UartCtrl()
{
    close();

}
void UartCtrl::close()
{
    mTerminateThread = true;
    while (mFd >=0) {
        usleep(15000);
    }
    if(mpReceiveBuffer){
        free(mpReceiveBuffer);
        mpReceiveBuffer = nullptr;
    }


}
static void* ReadThread(void* data)
{
    UartCtrl* This = (UartCtrl*)data;
    This->onReadCallback();

    return (void*)0;
}
#define RECEIVE_BUFFER_SIZE  1024
void UartCtrl::onReadCallback()
{
    mTerminateThread = false;

    if(mpReceiveBuffer == nullptr)
        mpReceiveBuffer = (char*) malloc(RECEIVE_BUFFER_SIZE);

    int flags = fcntl(mFd, F_GETFL, 0);
    fcntl(mFd, F_SETFL, flags | O_NONBLOCK);


    while(!mTerminateThread && mFd >= 0) {
        int c = read(mFd, mpReceiveBuffer, RECEIVE_BUFFER_SIZE);
        if (c > 0) {
            if(mfnReadCallback)
                mfnReadCallback(mpReceiveBuffer, c, mpUserData);
        }
        usleep(10000);
    }
    if(mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }

}

int UartCtrl::open(const char* dev, UartReadCallback fnOnRead, void* data)
{
    if(mFd >= 0)
        this->close();
    mfnReadCallback = fnOnRead;
    mpUserData = data;
    mFd = ::open (dev, O_RDWR | O_NOCTTY | O_DSYNC, S_IRUSR | S_IWUSR);
    if (-1 == mFd) {
        return mFd;
    }

    setBaudRate(mFd, mBaudRate);
    setParity(mFd, mDatabits, mStopbits, mParity, mHfc, mSfc);
    pthread_t mThreadId;
    pthread_create(&mThreadId, NULL, ReadThread, (void*) this);
    return mFd;
}
bool UartCtrl::setBaud(int nBaudRate)
{
    mBaudRate = nBaudRate;
    if(mFd <0)
        return true; //set baudrate after open
    return setBaudRate(mFd, mBaudRate);
}

bool UartCtrl::setAttrib(int databits,int stopbits,int parity, bool bHfc, bool bSfc)
{
    mDatabits = databits;
    mStopbits = stopbits;
    mParity = parity;
    mHfc = bHfc;
    mSfc = bSfc;
    if(mFd < 0)
        return true; //not open device yet. Set latter.
    return (0 == setParity(mFd, mDatabits, mStopbits, mParity, mHfc, mSfc));
}
size_t UartCtrl::write(const void* buffer, size_t length)
{
    if (mFd < 0)
        return -1;
    return ::write(mFd, buffer, length);
}
