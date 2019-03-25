#include "controlpanel.h"
#include "ui_controlpanel.h"

#include "mainwindow.h"
ControlPanel::ControlPanel(QWidget *parent) :
    QWidget(parent),ui(new Ui::ControlPanel)
{

    ui->setupUi(this);
    connect(ui->btnSend, SIGNAL(clicked()), SLOT(onSend()));


}

ControlPanel::~ControlPanel()
{
    delete ui;
}
void ControlPanel::onSend()
{
    QString text  = ui->message->toPlainText() + "\r";

    gMainWnd->WriteMessage(text.toStdString().c_str(), text.size());
}
