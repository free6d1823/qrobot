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
    void updateUi(QString port, int baud, bool eightbits, bool par, bool one, bool hfc, bool sfc);
private:
    void pollingComPort();
    Ui::ComSetupDlg *ui;
    QStringList* mpPortList;
public:
    QString mPort;
    int mBaud;  //number, bits per second
    bool mEightbits;
    bool mPar;
    bool mOne;
    bool mHfc;
    bool mSfc;

signals:
public slots:
    void onClickPortList(int);
    void onOpen();

};

#endif // COMSETUPDLG_H
