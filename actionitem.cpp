#include "actionitem.h"
#include "ui_actionitem.h"

ActionItem::ActionItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ActionItem)
{
    ui->setupUi(this);
}

ActionItem::~ActionItem()
{
    delete ui;
}
