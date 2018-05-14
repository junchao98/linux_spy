#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "qt_aes.h"

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
      QTcpSocket *clientConnection;
       TAesClass *aes = new TAesClass;

private slots:

    void readMessage();  //接收数据

    void on_pushButton_clicked();
    void sendMessage(char* s_data);
    void send_init_message();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;

    QString message;  //存放从服务器接收到的字符串
    quint16 blockSize; //存放文件的大小信息
};

#endif // MAINWINDOW_H
