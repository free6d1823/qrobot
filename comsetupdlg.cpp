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
    230400, 115200, 57600,  19200,  9600,  4800
};



ComSetupDlg::ComSetupDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ComSetupDlg),
    mpPortList(new QStringList),
    mPort("/dev/ttyUSB0"), mBaud(115200), mDatabits(8), mParity('n'), mStopbits(1), mHfc(false), mSfc(false)
{
    ui->setupUi(this);

    for (size_t i=0; i< sizeof(baudList)/sizeof(baudList[0]); i++){
        ui->listBaud->addItem(QString::number(baudList[i]));
    }
    connect(ui->listPort, SIGNAL(activated(int)), SLOT(onClickPortList(int)));
    connect(ui->btnOK, SIGNAL(clicked()), SLOT(onOpen()));
    updateUi();
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
        mBaud = baudList[i];
    }
    mDatabits = (ui->db7->isChecked())?7:8;
    if (ui->peven->isChecked())
        mParity = 'e';
    else if (ui->podd->isChecked())
        mParity = 'o';
    else
        mParity = 'n';
    mStopbits = (ui->s1->isChecked() )?1:2;
    mHfc = (ui->hfc->isChecked() );
    mSfc = (ui->sfc->isChecked() );

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
void ComSetupDlg::set(QString port, int baud, int databits, int parity, int stopbits, bool hfc, bool sfc)
{
    pollingComPort();
    mPort = port;


    mBaud = baud;
    mStopbits = databits;
    mParity = parity;
    mStopbits = stopbits;
    mHfc = hfc;
    mSfc = sfc;
    updateUi();
}

void ComSetupDlg::updateUi()
{
    ui->listPort->setCurrentIndex(-1);
    for (int i=0; i< mpPortList->size(); i++){

        if (mPort.compare(mpPortList->at(i)) == 0){
            ui->listPort->setCurrentIndex(i);
            break;
        }
    }
    for (size_t i=0; i< sizeof(baudList)/sizeof(baudList[0]); i++){
        if (mBaud == baudList[i]) {
            ui->listBaud->setCurrentIndex(i);
            break;
        }
    }
    ui->db7->setChecked(mStopbits==7);
    ui->db8->setChecked(mStopbits!=7);
    switch(mParity)
    {
    case 'o':
        ui->podd->setChecked(1);ui->peven->setChecked(0);ui->pnone->setChecked(0);
        break;
    case 'e':
        ui->podd->setChecked(0);ui->peven->setChecked(1);ui->pnone->setChecked(0);
        break;
    default:
        ui->podd->setChecked(0);ui->peven->setChecked(0);ui->pnone->setChecked(1);
        break;


    }
    ui->s1->setChecked(mStopbits != 2);
    ui->s2->setChecked(mStopbits == 2);
    ui->hfc->setChecked(mHfc);
    ui->sfc->setChecked(mSfc);
}
