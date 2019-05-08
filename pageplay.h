#ifndef PAGEPLAY_H
#define PAGEPLAY_H

#include <QWidget>
#include <QToolButton>
#include "common.h"
#include "controlpage.h"
namespace Ui {
class PagePlay;
}

class PagePlay : public ControlPage
{
    Q_OBJECT

public:
    explicit PagePlay(QWidget *parent = nullptr);
    ~PagePlay();
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
    Ui::PagePlay *ui;
};

#endif // PAGEPLAY_H
