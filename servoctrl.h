#ifndef SERVOCTRL_H
#define SERVOCTRL_H

#include <QWidget>
#include "common.h"

namespace Ui {
class ServoCtrl;
}
class Servo;
class ServoCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit ServoCtrl(QWidget *parent, int i);
    ~ServoCtrl();
    ///
    /// \brief update UI with Servo data
    ///
    virtual void updateUi();
    ///
    /// \brief save UI to Servo object
    ///
    virtual void saveUi();
public slots:
    void onSlideChanged(int value);
private:
    int mId;
    Ui::ServoCtrl *ui;
};
class ServoPage : public QWidget
{
    Q_OBJECT

public:
    explicit ServoPage(QWidget *parent = 0);
    ~ServoPage();
    ///
    /// \brief activate the page
    /// \param enable true to activate, false to deactivate
    ///
    virtual void activate(bool enable);
    ///
    /// \brief save Ui to all Servo objects
    ///
    virtual void saveUi();
private:

    ServoCtrl* mpServoCtrl[MAX_SERVOS_NUMBER];

};

#endif // SERVOCTRL_H
