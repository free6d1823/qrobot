#ifndef ACTIONITEM_H
#define ACTIONITEM_H

#include <QWidget>
#include <QListWidgetItem>

namespace Ui {
class ActionItem;
}

class ActionItem : public QWidget
{
    Q_OBJECT

public:
    explicit ActionItem(QWidget *parent = 0);
    ~ActionItem();
public slots:
    void onGoClicked();
    void onEditClicked();

private:
    Ui::ActionItem *ui;
};
#endif // ACTIONITEM_H
