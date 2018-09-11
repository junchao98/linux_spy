#include "send_file.h"
#include "ui_send_file.h"

send_file::send_file(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::send_file)
{
    ui->setupUi(this);
}

send_file::~send_file()
{
    delete ui;
}



enum DOWN_TYPE{

    ST_PACK = 0,
    ST_FILE
};



void send_file::on_pushButton_ok_clicked()
{


       if( ui->checkBox_md5->isChecked()){
	   
			file_mark.enabled_md5 = true;

	   }


       file_mark.str_file_type = ui->comboBox_file_type->currentText();


       if(file_mark.str_file_type == "file"){


           file_mark.file_type = ST_FILE;

       }else{

           file_mark.file_type = ST_PACK;


       }



    close();

}
























