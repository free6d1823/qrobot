#include "comsetupdlg.h"
#include "ui_comsetupdlg.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <QMessageBox>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>


static int baudList[] = {
    19200, 38400, 57600, 115200,230400
};
static int BaudIdList[] = {
    B19200, B38400, B57600, B115200,B230400
};


ComSetupDlg::ComSetupDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComSetupDlg),
    mpPortList(new QStringList),
    mPort("/dev/ttyUSB0"), mBaud(B115200), mEightbits(true), mPar(false), mOne(true), mHfc(false), mSfc(false)
{
    ui->setupUi(this);

    for (size_t i=0; i< sizeof(baudList)/sizeof(baudList[0]); i++){
        ui->listBaud->addItem(QString::number(baudList[i]));
    }
    connect(ui->listPort, SIGNAL(activated(int)), SLOT(onClickPortList(int)));
    connect(ui->btnOK, SIGNAL(clicked()), SLOT(onOpen()));

}
void ComSetupDlg::onOpen()
{
    int i;

    if ( ((i = ui->listPort->currentIndex()) <  mpPortList->size()) && (i >= 0) )
    {
        mPort = mpPortList->at(i);
    } else {
        return;//no selection
    }

    if ((i = ui->listBaud->currentIndex()) <  (int) (sizeof(baudList)/sizeof(baudList[0])) && (i >= 0))
    {
        mBaud = BaudIdList[i];
    }
    mEightbits = ui->att1->isChecked();
    mPar = (ui->att2->isChecked() );
    mOne = (ui->att3->isChecked() );
    mHfc = (ui->att4->isChecked() );
    mSfc = (ui->att5->isChecked() );

    accept();
}

void ComSetupDlg::onClickPortList(int )
{
    pollingComPort();
}

void ComSetupDlg::pollingComPort()
{
    mpPortList->clear();
    char filename[32];
    struct stat   sb;
    for (int i=0; i<20; i++){
        sprintf(filename, "/dev/ttyUSB%d", i);
        if ( stat(filename, &sb) == 0) {
            mpPortList->append(filename);
        }

    }
    ui->listPort->clear();
    for (int i=0; i< mpPortList->size(); i++){
        ui->listPort->addItem(mpPortList->at(i));
    }
}

ComSetupDlg::~ComSetupDlg()
{
    delete ui;
    delete mpPortList;
}
/* baud = BaudRate ID Bnnn */
void ComSetupDlg::updateUi(QString port, int baud, bool eightbits, bool par, bool one, bool hfc, bool sfc)
{
    pollingComPort();
    mPort = port;
    ui->listPort->setCurrentIndex(-1);
    for (int i=0; i< mpPortList->size(); i++){

        if (mPort.compare(mpPortList->at(i)) == 0){
            ui->listPort->setCurrentIndex(i);
            break;
        }
    }
    for (size_t i=0; i< sizeof(baudList)/sizeof(baudList[0]); i++){
        if (baud == BaudIdList[i]) {
            ui->listBaud->setCurrentIndex(i);
            break;
        }
    }

    ui->att1->setChecked(eightbits);
    ui->att2->setChecked(par);
    ui->att3->setChecked(one);
    ui->att4->setChecked(hfc);
    ui->att5->setChecked(sfc);

    mBaud = baud;
    mEightbits = eightbits;
    mPar = par;
    mOne = one;
    mHfc = hfc;
    mSfc = sfc;

}

