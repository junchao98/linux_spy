#ifndef CHANGE_GID_H
#define CHANGE_GID_H

#include <QDialog>

namespace Ui {
class change_gid;
}

class change_gid : public QDialog
{
    Q_OBJECT

public:
    explicit change_gid(QWidget *parent = 0);
    ~change_gid();

private:
    Ui::change_gid *ui;
};

#endif // CHANGE_GID_H
