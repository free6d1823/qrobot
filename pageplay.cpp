#include "pageplay.h"
#include "ui_pageplay.h"

PagePlay::PagePlay(QWidget *parent) :
    ControlPage(parent),
    ui(new Ui::PagePlay)
{
    ui->setupUi(this);
}

PagePlay::~PagePlay()
{
    delete ui;
}

void PagePlay::activate(bool enable)
{

    setVisible(enable);
}
void PagePlay::saveUi()
{

}
