#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "qt_aes.h"
#include <QTimer>
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


    struct transfer_inf{

        QFile * localFile;
        qint64 totalBytes;  //数据总大小
        qint64 bytesWritten;  //已经发送数据大小
        qint64 bytesToWrite;   //剩余数据大小

    };

    struct m_client{

        int id ;
         struct transfer_inf file_inf;
        QTcpSocket *clientConnection;
    };


      QTimer *timer_down;

      QTcpSocket *clientConnection[10];
      int c_user_num;

      TAesClass *aes = new TAesClass;

       void readMessage(QTcpSocket * socket);  //接收数据
       void sendMessage(QTcpSocket * socket, char* s_data);
       void sendFile(struct m_client * p_clinet, QString fileName);



       QList<struct m_client *> msg_clinet_list;
       QList<struct m_client *> file_clinet_list;

        void email_data(char * data, uint8_t addr);
        void show_client(struct m_client * p_clinet );
        int verison;

private slots:

    void on_pushButton_clicked();
    void send_init_message();
     void send_init_file();
    void m_disconnect();
    void readsocket_data();
    void sendFileBody(qint64 numBytes);
    void auto_scanf_down(void);


private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer,
                *tcp_file_server;

    QByteArray outBlock;    //数据缓冲区，即存放每次要发送的数据


    qint64 loadSize;   //每次发送数据的大小

    QString message;  //存放从服务器接收到的字符串
    quint16 blockSize; //存放文件的大小信息


};

#endif // MAINWINDOW_H
