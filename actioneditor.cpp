#include "actioneditor.h"
#include "ui_actioneditor.h"
static void OnServoCtrlAngleChanged(int id, int angle, int speed, void* data)
{
    if(data) {
        ((ActionEditor*)data)->onAngleChanged(id, angle, speed);
    }
}
ActionEditor::ActionEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ActionEditor)
{
    ui->setupUi(this);
    connect(ui->spinTime, SIGNAL(valueChanged(int)),
            this, SLOT(onTotalTimeChanged(int)));
    for (int i=0; i< MAX_SERVOS_NUMBER; i++) {
        mpServoCtrl[i] = new ServoSetting(this, i, OnServoCtrlAngleChanged, false);
        ui->list->addWidget(mpServoCtrl[i] );
    }
}

ActionEditor::~ActionEditor()
{
    delete ui;
}
void ActionEditor::onAngleChanged(int id, int angle, int speed)
{

}
