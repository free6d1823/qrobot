#ifndef PAGESEQ_H
#define PAGESEQ_H

#include <QWidget>
#include <QToolButton>
#include <vector>
#include "common.h"
#include "controlpage.h"

namespace Ui {
class PageSeq;
}
class ActionItem;
class QListWidgetItem;
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
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);
private:
    Ui::PageSeq *ui;
    std::vector <ActionItem*> mItem;
};

#endif // PAGESEQ_H
