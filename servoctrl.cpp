#include <QVBoxLayout>
#include "common.h"
#include "servo.h"
#include "servoctrl.h"
#include "ui_servoctrl.h"
#include "pageservos.h"
#include "mainwindow.h"

ServoCtrl::ServoCtrl(QWidget *parent, int i, ServoCtrlAngleChanged callback, bool hideCheck) :
    QWidget(parent),mId(i),
    ui(new Ui::ServoCtrl)
{
    ui->setupUi(this);

    mParent = parent;
    mFnAngleChanged = callback;

    if (hideCheck)
        ui->check->setVisible(false);
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
    if(mFnAngleChanged)
    mFnAngleChanged(mId, value, ui->spinSpeed->value(), mParent);

}

void ServoCtrl::updateUi()
{
    Servo* ps = gSystem->getServo(mId);
    int port= ps->getPort();
    char* name = ps->getName();
    ui->groupBox->setTitle(QString::number(mId+1));
    ui->name->setText(name);
    ui->port->setText(QString::number(port));
    ui->spinAngle->setMaximum((int)ps->maxAngle());
    ui->spinAngle->setMinimum((int)ps->minAngle());
    ui->slideAngle->setMinimum((int)ps->minAngle());
    ui->slideAngle->setMaximum((int)ps->maxAngle());
    ui->spinAngle->setValue((int)ps->currentAngle());
    ui->slideAngle->setValue((int)ps->currentAngle());
    ui->spinSpeed->setValue((int)ps->currentSpeed());
}
void ServoCtrl::saveUi()
{
    Servo* ps = gSystem->getServo(mId);
    ps->setCurrentAngle((float) ui->spinAngle->value());
    ps->setCurrentSpeed((float) ui->spinSpeed->value());

}
