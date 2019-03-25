#include "mainview.h"

MainView::MainView(QWidget *parent) : QWidget(parent)
{
    QPalette p = palette();

    p.setColor(QPalette::Active, QPalette::Base, Qt::yellow);
    p.setColor(QPalette::Inactive, QPalette::Base, Qt::red);

}
