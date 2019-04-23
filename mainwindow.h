#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mainview.h"
#include "console.h"


#include <QDebug>
#include <QMetaType>
#include <QStringList>

class Message
{
public:
    Message();
    Message(const Message &other);
    ~Message();

    Message(const QString &body, const QStringList &headers);

    QString body() const;
    QStringList headers() const;

private:
    QString m_body;
    QStringList m_headers;
};

Q_DECLARE_METATYPE(Message);
/////////////////////////////
namespace Ui {
class MainWindow;
}

class ControlPanel;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int WriteMessage(const char* message, int length);

private:
    Ui::MainWindow *ui;
    void createMenuAndToolbar();
    void createUi();
    void updateUi();

public slots:
     void onFileOpen();
     void onFileClear();
     void about();

private:
     QAction *mOpenAct;
     QAction *mCloseAct;
     QAction *mClearAct;

     QDockWidget* mpDockView;
     ConsleView* mpConsoleView;
    ControlPanel* mpControlPanel;
    MainView*   mpMainView;

protected:
    virtual void resizeEvent(QResizeEvent *event);
    ////////////////////////////////////////////////////////////////
    virtual void customEvent(QEvent* event);
    static const QEvent::Type MY_CUSTOM_EVENT = static_cast<QEvent::Type>(QEvent::User + 1);

    // Define your custom event subclass
    class UpdateMEssageEvent : public QEvent
    {
        public:
            UpdateMEssageEvent(const char* message, int length):
                QEvent(MY_CUSTOM_EVENT),
                mPtrMessage(message),
                mLength(length)
            {
            }

            const char* getMessage() const
            {
                return mPtrMessage;
            }

            int getLength() const
            {
                return mLength;
            }

        private:
            const char* mPtrMessage;
            int mLength;
    };
public:
    void postUpdateMessageEvent(const char* message, int length);
private:
    void handleUpdateMessageEvent(const UpdateMEssageEvent *event);
};



extern MainWindow* gMainWnd;
#endif // MAINWINDOW_H
