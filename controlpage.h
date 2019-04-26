#ifndef CONTROLPAGE_H
#define CONTROLPAGE_H

#include <QWidget>

class ControlPage : public QWidget
{
    Q_OBJECT
public:
    explicit ControlPage(QWidget *parent);
    virtual ~ControlPage();
    ///
    /// \brief activate the page
    /// \param enable true to activate, false to deactivate
    ///
    virtual void activate(bool ){};
    ///
    /// \brief save Ui to all Servo objects
    ///
    virtual void saveUi(){};
};

#endif // CONTROLPAGE_H
