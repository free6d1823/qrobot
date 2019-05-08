#include <QVBoxLayout>
#include "pageservo.h"
#include "ui_servoctrl.h"
#include "mainwindow.h"

ServoCtrl::ServoCtrl(QWidget *parent, int i) :
    QWidget(parent),mId(i),
    ui(new Ui::ServoCtrl)
{
    ui->setupUi(this);

    mParent = (PageServo*) parent;
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
    char command[256];

    Servo* ps = gMainWnd->getServo(mId);
    int port = ps->getPort();
    int spd = ui->spinSpeed->value();

    int time = mParent->getServoTime();
    if (spd > 0) {
        sprintf(command, "#%dP%dS%dT%d\n", port, ps->findPwByAngle((float)value),
            spd, time);
    }else {
        sprintf(command, "#%dP%d T%d\n", port, ps->findPwByAngle((float)value),
            time);

    }

    gMainWnd->WriteMessage(command, (int)strlen(command));
    gMainWnd->appendLog(command);
}

void ServoCtrl::updateUi()
{
    Servo* ps = gMainWnd->getServo(mId);
    int port= ps->getPort();
    char* name = ps->getName();
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
    QHBoxLayout* timeLayout = new QHBoxLayout(this)  ;
    QLabel* dur = new QLabel(this);
    dur->setPixmap(QPixmap(":/images/time.png"));
    QLabel* unit= new QLabel("ms", this);
    mSpinTime = new QSpinBox(this);
    mSpinTime->setRange(0,65535);
    mSpinTime->setSingleStep(100);
    mSpinTime->setValue(1000);
    timeLayout->addWidget(dur);
    timeLayout->addWidget(mSpinTime);
    timeLayout->addWidget(unit);
     mainLayout->addLayout(timeLayout );
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
