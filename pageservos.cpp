#include "pageservos.h"
#include "ui_pageservos.h"

#include "mainwindow.h"

static void OnServoCtrlAngleChanged(int id, int angle, int speed, void* data)
{
    if(data) {
        ((PageServos*)data)->onAngleChanged(id, angle, speed);
    }
}

PageServos::PageServos(QWidget *parent) :
    ControlPage(parent),
    ui(new Ui::PageServos)
{
    ui->setupUi(this);
    connect(ui->spinTime, SIGNAL(valueChanged(int)),
            this, SLOT(onTotalTimeChanged(int)));
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpServoCtrl[i] = new ServoCtrl(this, i, OnServoCtrlAngleChanged, true);
        ui->list->addWidget(mpServoCtrl[i] );
    }
}


PageServos::~PageServos()
{
    delete ui;
}
/*
int PageServos::getServoTime()
{
    return ui->spinTime->value();
}*/
void PageServos::onTotalTimeChanged(int value)
{
    gSystem->setSingleTime(value);
}

void PageServos::onAngleChanged(int id, int angle, int speed)
{
        if (isVisible()){
    char command[256];
    Servo* ps = gSystem->getServo(id);
    int port = ps->getPort();
    //int spd = 1000;//ui->spinSpeed->value();

    int time = ui->spinTime->value();
    if (speed > 0) {
        sprintf(command, "#%dP%dS%dT%d\n", port, ps->findPwByAngle((float)angle),
            speed, time);
    }else {
        sprintf(command, "#%dP%d T%d\n", port, ps->findPwByAngle((float)angle),
            time);

    }

    gMainWnd->WriteMessage(command, (int)strlen(command));
    gMainWnd->appendLog(command);
    }
}

void PageServos::activate(bool enable)
{
    if (enable) {
        ui->spinTime->setValue(gSystem->singleTime());
        for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
            mpServoCtrl[i]->updateUi();
        }
    }
    setVisible(enable);
}
void PageServos::saveUi()
{
    gSystem->setSingleTime(ui->spinTime->value());
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpServoCtrl[i]->saveUi();
    }
}
