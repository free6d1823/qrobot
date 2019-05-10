#ifndef PAGESERVOS_H
#define PAGESERVOS_H

#include <QWidget>
#include "common.h"
#include "controlpage.h"
#include "servoctrl.h"
namespace Ui {
class PageServos;
}

class PageServos : public ControlPage
{
    Q_OBJECT

public:
    explicit PageServos(QWidget *parent = 0);
    virtual ~PageServos();
    ///
    /// \brief activate the page
    /// \param enable true to activate, false to deactivate
    ///
    virtual void activate(bool enable);
    ///
    /// \brief save Ui to all Servo objects
    ///
    virtual void saveUi();

//    int getServoTime();
    void onAngleChanged(int id, int angle, int speed);

public slots:
    void onTotalTimeChanged(int value);
private:
    Ui::PageServos *ui;
    ServoCtrl* mpServoCtrl[MAX_SERVOS_NUMBER];
};

#endif // PAGESERVOS_H
