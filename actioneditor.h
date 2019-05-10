#ifndef ACTIONEDITOR_H
#define ACTIONEDITOR_H

#include <QDialog>
#include "common.h"
#include "servoctrl.h"
namespace Ui {
class ActionEditor;
}

class ActionEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ActionEditor(QWidget *parent = 0);
    ~ActionEditor();
    void onAngleChanged(int id, int angle, int speed);
private:
    Ui::ActionEditor *ui;
    ServoSetting* mpServoCtrl[MAX_SERVOS_NUMBER];
};

#endif // ACTIONEDITOR_H
