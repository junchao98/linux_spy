#include "change_gid.h"
#include "ui_change_gid.h"

#include <QMessageBox>


change_gid::change_gid(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::change_gid)
{
    ui->setupUi(this);
}

change_gid::~change_gid()
{
    delete ui;
}

void change_gid::on_pushButton_ok_clicked()
{

    if(ui->lineEdit_gid->text().isEmpty()){

         QMessageBox::information(this,"错误","请输入组ID");
        return;

    }

    new_gid = ui->lineEdit_gid->text();

     close();

}

void change_gid::on_pushButton_cancel_clicked()
{
    new_gid.clear();

      close();
}
