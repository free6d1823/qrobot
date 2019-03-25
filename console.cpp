#include "console.h"
#include <QScrollBar>
#include <QStyleOption>
Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
      QPalette p = palette();

      p.setColor(QPalette::Active, QPalette::Base, Qt::red);
      p.setColor(QPalette::Inactive, QPalette::Base, Qt::red);
/*
    QPalette pal = palette();

    // set black background
    pal.setColor(QPalette::Background, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);
    show();
    //_myWidget->
    setStyleSheet("background-color:black;");
    show();
    */

}

Console::~Console()
{

}

void Console::appendMessage(const QString& text)
{
    this->appendPlainText(text); // Adds the message to the widget
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
}

void Console::resizeEvent(QResizeEvent *event)
{

    QWidget * pParent = parentWidget();
    QRect rc = pParent->geometry();
    setGeometry(rc);
}
void Console::paintEvent(QPaintEvent* event)
{
    /*
 QStyleOption opt;
 opt.init(this);

 QPainter* p= new QPainter(this);
 style()->drawPrimitive(QStyle::PE_Widget, &opt, p, this);
*/
 QWidget::paintEvent(event);
}
