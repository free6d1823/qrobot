#ifndef CALIPAGE_H
#define CALIPAGE_H

#include <QWidget>
#include "common.h"

namespace Ui {
class Calibrate;
}

class Calibrate : public QWidget
{
    Q_OBJECT

public:
    explicit Calibrate(QWidget *parent, int id);
    ~Calibrate();
    ///
    /// \brief update UI with Servo data
    ///
    virtual void updateUi();
    ///
    /// \brief save UI to Servo object
    ///
    virtual void saveUi();
private:
    int mId;
    Ui::Calibrate *ui;
};


class CaliPage : public QWidget
{
    Q_OBJECT

public:
    explicit CaliPage(QWidget *parent = 0);
    ~CaliPage();
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

    Calibrate* mpCali[MAX_SERVOS_NUMBER];
};

#endif // CALIPAGE_H
