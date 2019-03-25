#ifndef CONTROLPANEL_H
#define CONTROLPANEL_H

#include <QWidget>

namespace Ui {
class ControlPanel;
}

class ControlPanel : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPanel(QWidget *parent = 0);
    ~ControlPanel();
private:
    Ui::ControlPanel *ui;


signals:

public slots:
    void onSend();

};

#endif // CONTROLPANEL_H
