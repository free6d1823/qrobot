#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include <QtWidgets/qscrollarea.h>

class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit Console(QWidget *parent = 0);
    ~Console();
    void appendMessage(const QString& text);

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual void paintEvent(QPaintEvent* event);

};


#endif // CONSOLE_H
