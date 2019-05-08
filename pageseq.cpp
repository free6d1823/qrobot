#include "pageseq.h"
#include "ui_pageseq.h"

PageSeq::PageSeq(QWidget *parent) :
    ControlPage(parent),
    ui(new Ui::PageSeq)
{
    ui->setupUi(this);
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
