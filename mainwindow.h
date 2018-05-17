#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "qt_aes.h"

#include <qlist.h>

extern "C"{

#include "msg_core/common.h"
#include "msg_core/list.h"
#include "msg_core/msg_core.h"


}


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    UCHAR aes_key[16] = {0x22, 0xa2, 0x13, 0xe2, 0x22, 0xa2, 0x12, 0x20, 0x22, 0x15, 0x22, 0xda, 0x67, 0x22, 0x32, 0x22 };

      QTcpSocket *clientConnection[10];
      int c_user_num;

      TAesClass *aes = new TAesClass;
       void readMessage(QTcpSocket * socket);  //接收数据
        void sendMessage(QTcpSocket * socket, char* s_data);

private slots:

    void on_pushButton_clicked();

    void send_init_message();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;

    QString message;  //存放从服务器接收到的字符串
    quint16 blockSize; //存放文件的大小信息
};

#endif // MAINWINDOW_H
