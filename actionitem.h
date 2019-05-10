#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <QWidget>

namespace Ui {
class ActionItem;
}

class ActionItem : public QWidget
{
    Q_OBJECT

public:
    explicit ActionItem(QWidget *parent = 0);
    ~ActionItem();

private:
    Ui::ActionItem *ui;
};

#endif // ACTIONITEM_H
