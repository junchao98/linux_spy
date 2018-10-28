#ifndef SEND_FILE_H
#define SEND_FILE_H

#include <QDialog>

namespace Ui {
class send_file;
}

class send_file : public QDialog
{
    Q_OBJECT

public:
    explicit send_file(QWidget *parent = 0);
    ~send_file();

    struct _file_mark{

        bool enabled_md5;
        int file_type;
        QString str_file_type;

    };


    struct _file_mark file_mark;

private slots:
    void on_pushButton_ok_clicked();

private:
    Ui::send_file *ui;
};

#endif // SEND_FILE_H
