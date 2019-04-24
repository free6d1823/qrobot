#ifndef COMSETUPDLG_H
#define COMSETUPDLG_H

#include <QDialog>

namespace Ui {
class ComSetupDlg;
}

class ComSetupDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ComSetupDlg(QWidget *parent = 0);
    ~ComSetupDlg();
    void set(QString port, int baud, int databits, int parity, int stopbits, bool hfc, bool sfc);
private:
    void pollingComPort();
    void updateUi();
    Ui::ComSetupDlg *ui;
    QStringList* mpPortList;
public:
    QString mPort;
    int mBaud;  //number, bits per second
    int mDatabits;  //7/8
    int mParity;    //'n','o','e'
    int mStopbits;  //1,2
    bool mHfc;      //true to enable hardware flow control
    bool mSfc;      //true to enable software flow control

signals:
public slots:
    void onClickPortList(int);
    void onOpen();

};

#endif // COMSETUPDLG_H
