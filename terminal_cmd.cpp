#include "terminal_cmd.h"
#include "ui_terminal_cmd.h"

terminal_cmd::terminal_cmd(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::terminal_cmd)
{
    ui->setupUi(this);

    ui->textEdit->installEventFilter(this);//设置完后自动调用其eventFilter函数

}

terminal_cmd::~terminal_cmd()
{
    delete ui;
}





bool terminal_cmd::eventFilter(QObject *target, QEvent *event)
{
    if(target == ui->textEdit)
    {
        if(event->type() == QEvent::KeyPress)//回车键
        {
             QKeyEvent *k = static_cast<QKeyEvent *>(event);
             if(k->key() == Qt::Key_Return)
             {
                 //on_send_clicked();
                 qDebug() << "enter key";
                 return true;
             }
        }
    }
    return QWidget::eventFilter(target,event);
}



