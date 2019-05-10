#ifndef PAGESEQ_H
#define PAGESEQ_H

#include <QWidget>
#include <QToolButton>
#include "common.h"
#include "controlpage.h"

namespace Ui {
class PageSeq;
}

class PageSeq : public ControlPage
{
    Q_OBJECT

public:
    explicit PageSeq(QWidget *parent = nullptr);
    ~PageSeq();
    ///
    /// \brief activate the page
    /// \param enable true to activate, false to deactivate
    ///
    virtual void activate(bool enable);
    ///
    /// \brief save Ui to all Servo objects
    ///
    virtual void saveUi();
public slots:
    void onAddClicked();
    void onDeleteClicked();
    void onUpClicked();
    void onDownClicked();

private:
    Ui::PageSeq *ui;

};

#endif // PAGESEQ_H
