#ifndef TERMINAL_CMD_H
#define TERMINAL_CMD_H

#include <QDialog>
#include<QKeyEvent>
#include <QDebug>

namespace Ui {
class terminal_cmd;
}

class terminal_cmd : public QDialog
{
    Q_OBJECT

public:
    explicit terminal_cmd(QWidget *parent = 0);
    ~terminal_cmd();


protected:
    bool eventFilter(QObject *target, QEvent *event);//事件过滤器

public slots:

      get_terminal_bak(QString);


private:
    Ui::terminal_cmd *ui;
};

#endif // TERMINAL_CMD_H
