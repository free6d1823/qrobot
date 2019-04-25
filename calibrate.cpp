#include "calibrate.h"
#include "ui_calibrate.h"
#include "mainwindow.h"
#include <QComboBox>
#include <QLabel>


Calibrate::Calibrate(QWidget *parent, int id) :
    QWidget(parent), mId(id),
    ui(new Ui::Calibrate)
{
    ui->setupUi(this);
}

Calibrate::~Calibrate()
{
    delete ui;
}
void Calibrate::updateUi()
{
    Servo* ps = gMainWnd->getServo(mId);
    int port;
    char* name = ps->getPortName(&port);
    ui->id->setText(QString::number(mId+1));
    ui->name->setText(name);
    ui->port->setValue(port);
    ui->minAngle->setValue((int)ps->minAngle());
    ui->maxAngle->setValue((int)ps->maxAngle());
    ui->centerAngle->setValue((int)ps->centerAngle());
    ui->minPw->setValue((int)ps->minPw());
    ui->maxPw->setValue((int)ps->maxPw());
    ui->centerPw->setValue((int)ps->centerPw());
}
void Calibrate::saveUi()
{
    Servo* ps = gMainWnd->getServo(mId);
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
CaliPage::CaliPage(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this)  ;
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpCali[i] = new Calibrate(this, i);
        mainLayout->addWidget(mpCali[i] );
    }
    setLayout(mainLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);

}

CaliPage::~CaliPage()
{

}
void CaliPage::activate(bool enable)
{
    if (enable) {
        for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
            mpCali[i]->updateUi();
        }
    }
    setVisible(enable);
}
void CaliPage::saveUi()
{
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpCali[i]->saveUi();
    }
}
