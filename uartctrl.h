#ifndef UARTCTRL_H
#define UARTCTRL_H

typedef void (*UartReadCallback)(const void* buffer, size_t length, void* pUserData);
class UartCtrl
{
public:
    UartCtrl();
    ~UartCtrl();
    int open(const char* dev, UartReadCallback fnOnRead, void* data);
    bool isOpened(){ return (mFd >= 0);}
    void close();
    bool setBaud(int nBaudRate=115200);
    bool setAttrib(int databits=8,int stopbits=1,int parity='n', bool bHfc=false, bool bSfc=false);
    size_t write(const void* buffer, size_t length);
    //callback function
    void onReadCallback();
private:

    bool mTerminateThread;
    int mFd;
    int mBaudRate;
    int mDatabits;
    int mStopbits;
    int mParity;
    bool mHfc;
    bool mSfc;
    UartReadCallback mfnReadCallback;
    void*   mpUserData;
    char* mpReceiveBuffer;
};

#endif // UARTCTRL_H
