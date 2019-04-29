#include "pagecali.h"
#include "ui_calibrate.h"
#include "mainwindow.h"
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>

Calibrate::Calibrate(QWidget *parent ) :
    QWidget(parent),
    ui(new Ui::Calibrate)
{
    ui->setupUi(this);
}

Calibrate::~Calibrate()
{
    delete ui;
}
void Calibrate::updateUi(int id)
{
    Servo* ps = gMainWnd->getServo(id);
    int port;
    char* name = ps->getPortName(&port);
    ui->id->setText(QString::number(id+1));
    ui->name->setText(name);
    ui->port->setValue(port);
    ui->minAngle->setValue((int)ps->minAngle());
    ui->maxAngle->setValue((int)ps->maxAngle());
    ui->centerAngle->setValue((int)ps->centerAngle());
    ui->minPw->setValue((int)ps->minPw());
    ui->maxPw->setValue((int)ps->maxPw());
    ui->centerPw->setValue((int)ps->centerPw());
}
void Calibrate::saveUi(int id)
{
    Servo* ps = gMainWnd->getServo(id);
    ps->setPortName(ui->name->text().toLatin1().data(),
                    ui->port->text().toInt());
    ps->setMinAngle((float)ui->minAngle->value());
    ps->setMaxAngle((float)ui->maxAngle->value());
    ps->setCenterAngle((float)ui->centerAngle->value());
    ps->setMinPw((float)ui->minPw->value());
    ps->setMaxPw((float)ui->maxPw->value());
    ps->setCenterPw((float)ui->centerPw->value());

}
///////////////////////////////
PageCali::PageCali(QWidget *parent) :
    ControlPage(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this)  ;

    QButtonGroup* btnGroup = new QButtonGroup;

    QHBoxLayout* btnLayout1 = new QHBoxLayout(this)  ;
    int i;
    mIconOff[0].addFile(":/images/no1_off.png");
    mIconOn[0].addFile(":/images/no1_on.png");
    mIconOff[1].addFile(":/images/no2_off.png");
    mIconOn[1].addFile(":/images/no2_on.png");
    mIconOff[2].addFile(":/images/no3_off.png");
    mIconOn[2].addFile(":/images/no3_on.png");
    mIconOff[3].addFile(":/images/no4_off.png");
    mIconOn[3].addFile(":/images/no4_on.png");
    mIconOff[4].addFile(":/images/no5_off.png");
    mIconOn[4].addFile(":/images/no5_on.png");
    mIconOff[5].addFile(":/images/no6_off.png");
    mIconOn[5].addFile(":/images/no6_on.png");
    for (i=0; i< MAX_SERVOS_NUMBER ; i++) {

        mpBtnServo[i] = new QToolButton(this);

        mpBtnServo[i]->setCheckable(true);
        mpBtnServo[i]->setIconSize(QSize(32,32));
        mpBtnServo[i]->setIcon(mIconOff[i]);
        btnLayout1->addWidget(mpBtnServo[i] );
        btnGroup->addButton(mpBtnServo[i], i);
        mpBtnServo[i]->setChecked(false);

    }
    mainLayout->addLayout(btnLayout1);

    connect(btnGroup, SIGNAL(buttonPressed(int)), this, SLOT(onBtnSelected(int)));
    btnGroup->setExclusive(true);
    mCurSel = 0;
    mpBtnServo[mCurSel]->setChecked(true);


    mpCali = new Calibrate(this);
    mainLayout->addWidget(mpCali );
    setLayout(mainLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    onBtnSelected(0);
}

PageCali::~PageCali()
{

}

void PageCali::onBtnSelected(int id)
{
    if (mCurSel != id && mCurSel < MAX_SERVOS_NUMBER)
        mpCali->saveUi(mCurSel);
    for (int i=0; i< MAX_SERVOS_NUMBER ; i++) {
        mpBtnServo[i]->setIcon( (id==i)?mIconOn[i]:mIconOff[i]);
    }
    mCurSel = id;
    mpCali->updateUi(mCurSel);
}

void PageCali::activate(bool enable)
{
    if (enable) {
        mpCali->updateUi(mCurSel);
    }
    setVisible(enable);
}
void PageCali::saveUi()
{
    mpCali->saveUi(mCurSel);
}
