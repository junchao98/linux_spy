#include "change_gid.h"
#include "ui_change_gid.h"

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
