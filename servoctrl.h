#ifndef SERVOSETTING_H
#define SERVOSETTING_H

#include <QWidget>

namespace Ui {
class ServoCtrl;
}

typedef void (*ServoCtrlAngleChanged)(int id, int angle, int speed, void* data);
class ServoCtrl : public QWidget
{
    Q_OBJECT

public:
    explicit ServoCtrl(QWidget *parent, int i, ServoCtrlAngleChanged callback, bool hideCheck=true);
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
    QWidget* mParent;
    ServoCtrlAngleChanged mFnAngleChanged;
    int mId;
    Ui::ServoCtrl *ui;
};

#endif // SERVOSETTING_H
