#include "actionitem.h"
#include "ui_actionitem.h"
#include "actioneditor.h"

ActionItem::ActionItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActionItem)
{
    ui->setupUi(this);
    connect(ui->go, SIGNAL(clicked()), this, SLOT(onGoClicked()));
     connect(ui->edit, SIGNAL(clicked()), this, SLOT(onEditClicked()));
}

ActionItem::~ActionItem()
{
    delete ui;
}
void ActionItem::onGoClicked()
{

}
void ActionItem::onEditClicked()
{
    ActionEditor dlg;
    dlg.exec();
}
