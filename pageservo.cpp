#include <QVBoxLayout>
#include "pgeservo.h"
#include "ui_servoctrl.h"
#include "mainwindow.h"

ServoCtrl::ServoCtrl(QWidget *parent, int i) :
    QWidget(parent),mId(i),
    ui(new Ui::ServoCtrl)
{
    ui->setupUi(this);

    connect(ui->spinAngle, SIGNAL(valueChanged(int)),
            ui->slideAngle, SLOT(setValue(int)));
    connect(ui->slideAngle, SIGNAL(valueChanged(int)),
            this, SLOT(onSlideChanged(int)));
    updateUi();
}

ServoCtrl::~ServoCtrl()
{
    delete ui;
}

void ServoCtrl::onSlideChanged(int value)
{
    ui->spinAngle->setValue(value);
}

void ServoCtrl::updateUi()
{
    Servo* ps = gMainWnd->getServo(mId);
    int port;
    char* name = ps->getPortName(&port);
    ui->name->setText(name);
    ui->port->setText(QString::number(port));
    ui->spinAngle->setMaximum((int)ps->maxAngle());
    ui->spinAngle->setMinimum((int)ps->minAngle());
    ui->slideAngle->setMinimum((int)ps->minAngle());
    ui->slideAngle->setMaximum((int)ps->maxAngle());
    ui->spinAngle->setValue((int)ps->currentAngle());
    ui->slideAngle->setValue((int)ps->currentAngle());
}
void ServoCtrl::saveUi()
{
    Servo* ps = gMainWnd->getServo(mId);
    ps->setCurrentAngle((float) ui->spinAngle->value());

}
///////////////////////////////
PageServo::PageServo(QWidget *parent) :
    ControlPage(parent)
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this)  ;
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpServoCtrl[i] = new ServoCtrl(this, i);
        mainLayout->addWidget(mpServoCtrl[i] );
    }

    setLayout(mainLayout);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);


}

PageServo::~PageServo()
{

}


void PageServo::activate(bool enable)
{
    if (enable) {
        for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
            mpServoCtrl[i]->updateUi();
        }
    }
    setVisible(enable);
}
void PageServo::saveUi()
{
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpServoCtrl[i]->saveUi();
    }
}
