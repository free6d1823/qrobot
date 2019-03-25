#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "mainview.h"
#include "console.h"

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
    int openCom();
    void closeCom();
    int WriteMessage(const char* message, int length);
private:
    Ui::MainWindow *ui;
    void createMenuAndToolbar();
    void createUi();
    void updateUi();
public slots:
     void onFileOpen();
     void onFileClear();
private:
     QAction *mOpenAct;
     QAction *mCloseAct;
     QAction *mClearAct;

     QDockWidget* mpDockView;
     QDockWidget* mpDockView2;
    ControlPanel* mpControlPanel;
    MainView*   mpMainView;
    Console*    mpConsole;

protected:
    virtual void resizeEvent(QResizeEvent *event);

};
extern MainWindow* gMainWnd;
#endif // MAINWINDOW_H
