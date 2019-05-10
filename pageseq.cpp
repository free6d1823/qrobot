#include "pageseq.h"
#include "ui_pageseq.h"
#include "actioneditor.h"
#include "actionitem.h"

PageSeq::PageSeq(QWidget *parent) :
    ControlPage(parent),
    ui(new Ui::PageSeq)
{
    ui->setupUi(this);
    connect(ui->add, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    connect(ui->remove, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    connect(ui->up, SIGNAL(clicked()), this, SLOT(onUpClicked()));
    connect(ui->down, SIGNAL(clicked()), this, SLOT(onDownClicked()));
    mItem.clear();
    for (int i=0; i<30; i++) {

        ActionItem* item = new ActionItem(this);
        mItem.push_back(item);
        QListWidgetItem* witem = new QListWidgetItem(QIcon(":/images/go.png"), "name");
        ui->list->addItem(witem);
    }
}

PageSeq::~PageSeq()
{
    delete ui;
}

void PageSeq::activate(bool enable)
{

    setVisible(enable);
}
void PageSeq::saveUi()
{

}
void PageSeq::onAddClicked()
{
    ActionEditor dlg;
    dlg.exec();
}

void PageSeq::onDeleteClicked()
{

}

void PageSeq::onUpClicked()
{

}

void PageSeq::onDownClicked()
{

}
