#include "pageseq.h"
#include "ui_pageseq.h"
#include "actioneditor.h"


PageSeq::PageSeq(QWidget *parent) :
    ControlPage(parent),
    ui(new Ui::PageSeq)
{
    ui->setupUi(this);
    connect(ui->add, SIGNAL(clicked()), this, SLOT(onAddClicked()));
    connect(ui->remove, SIGNAL(clicked()), this, SLOT(onDeleteClicked()));
    connect(ui->up, SIGNAL(clicked()), this, SLOT(onUpClicked()));
    connect(ui->down, SIGNAL(clicked()), this, SLOT(onDownClicked()));
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
