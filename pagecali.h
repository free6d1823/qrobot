#ifndef CALIPAGE_H
#define CALIPAGE_H

#include <QWidget>
#include <QToolButton>
#include <QButtonGroup>
#include "common.h"
#include "controlpage.h"
namespace Ui {
class Calibrate;
}

class PageCali;
class Calibrate : public QWidget
{
    Q_OBJECT

public:
    explicit Calibrate(QWidget *parent );
    ~Calibrate();
    ///
    /// \brief update UI with Servo data
    ///
    void updateUi(int id);
    ///
    /// \brief save UI to Servo object
    ///
    void saveUi(int id);
    void sendServoCommand(int idServo, int pw);
public slots:
    void onMinPwChanged(int value);
    void onMaxPwChanged(int value);
    void onCenterPwChanged(int value);
private:
    Ui::Calibrate *ui;
    PageCali* mParent;
};


class PageCali : public ControlPage
{
    Q_OBJECT

public:
    explicit PageCali(QWidget *parent = 0);
    ~PageCali();
    ///
    /// \brief activate the page
    /// \param enable true to activate, false to deactivate
    ///
    virtual void activate(bool enable);
    ///
    /// \brief save Ui to all Servo objects
    ///
    virtual void saveUi();
    int getCurrentServo(){ return mCurSel;}
public slots:
     void onBtnSelected(int id);
private:

    QIcon mIconOn[MAX_SERVOS_NUMBER];
    QIcon mIconOff[MAX_SERVOS_NUMBER];

    QToolButton* mpBtnServo[MAX_SERVOS_NUMBER];
    Calibrate* mpCali;
    int mCurSel;
};

#endif // CALIPAGE_H
