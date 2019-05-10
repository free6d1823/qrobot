#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "common.h"
#include "mainview.h"
#include "console.h"
#include "servo.h"
#include "uartctrl.h"
/////////////////////////////
namespace Ui {
class MainWindow;
}

class ControlPage;

class UartCtrl;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    int WriteMessage(const char* message, int length);
    void appendLog(char* log);
    bool loadSettings(const char* szIniName);
    bool saveSettings(const char* szIniName);
    UartCtrl* getUart(){ return &mUartCtrl;}


private:
    Ui::MainWindow *ui;
    void createMenuAndToolbar();
    void createUi();
    void updateUi();

public slots:
     void onFileOpen();
     void onFileSave();
     void onFileConnect();

     void onViewClear();
     void onZoomin(){};
     void onZoomout(){};
     void about();
     void onPage0();
     void onPage1();
     void onPage2();
     void onPage3();
     void onCommandHelp();
private:
    QAction *mOpenAct;
    QAction *mSaveAct;
    QAction *mConAct;
    QAction *mDisAct;
    QAction *mClearAct;
    QAction *mZoominAct;
    QAction *mZoomoutAct;
    QAction *mPageAct[MAX_PAGES];
    QIcon   mPageOn[MAX_PAGES];
    QIcon   mPageOff[MAX_PAGES];
    int mCurrentPageId;
    ControlPage* mpCtlPage[MAX_PAGES];

    QDockWidget* mpDockView;
    ConsleView* mpConsoleView;
    UartCtrl mUartCtrl;

    MainView*   mpMainView;

protected:
    void doEnablePage(int id);
    virtual void resizeEvent(QResizeEvent *event);
    ////////////////////////////////////////////////////////////////
    virtual void customEvent(QEvent* event);
    static const QEvent::Type MY_CUSTOM_EVENT = static_cast<QEvent::Type>(QEvent::User + 1);

    // Define your custom event subclass
    class UpdateMEssageEvent : public QEvent
    {
        public:
            UpdateMEssageEvent(char* message, int length):
                QEvent(MY_CUSTOM_EVENT),
                mPtrMessage(message),
                mLength(length)
            {
            }

            char* getMessage() const
            {
                return mPtrMessage;
            }

            int getLength() const
            {
                return mLength;
            }

        private:
            char* mPtrMessage;
            int mLength;
    };
public:
    void postUpdateMessageEvent(const char* message, int length);
private:
    void handleUpdateMessageEvent(UpdateMEssageEvent *event);
};






#endif // MAINWINDOW_H
