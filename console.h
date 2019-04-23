#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include <QObject>
#include <QDockWidget>
QT_BEGIN_NAMESPACE
class QPaintEvent;
class QResizeEvent;
class QSize;
class QWidget;
QT_END_NAMESPACE


//![codeeditordefinition]
//![extraarea]
class LineNumberArea;


class Console : public QPlainTextEdit
{
    Q_OBJECT

public:
    Console(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();
    void appendMessage(const char* text);
protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *lineNumberArea;


};
class LineNumberArea : public QWidget
{
public:
    LineNumberArea(Console *editor) : QWidget(editor) {
        mConsole = editor;
    }

    QSize sizeHint() const override {
        return QSize(mConsole->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        mConsole->lineNumberAreaPaintEvent(event);
    }

private:
    Console *mConsole;
};

class ConsleView : public QDockWidget
{
    Q_OBJECT

public:
    ConsleView(QWidget *parent = 0);
    void appendMessage(const char* text);
    /*!< clear content */
    void clear();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    Console *mpConsole;
};

#endif // CONSOLE_H
