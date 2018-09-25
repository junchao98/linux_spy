#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qt_aes.h"
#include "xml.h"

#include <QMainWindow>
#include <QtNetwork>
#include <QTimer>
#include <qlist.h>

#include <QtXml>
#include <QtXml/QDomDocument>


#include "baidu_map.h"
#include "send_file.h"
#include "terminal_cmd.h"




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


    struct _down_info{

        int status;     //是否执行下载
        qint64 type;       //下载类型
        QString file_path;
    };

    struct _bd_info{

        QString cpu_info,
                hd_info,
                client_id;

    };


    struct m_client{

         QString id ;
         QString group_id;

         struct _bd_info bd_info;
         struct transfer_inf file_inf;
         struct _down_info down_info;

         QTcpSocket *clientConnection;
    };

     struct _down_info down_info;

      QTimer *timer_down;

      QTcpSocket *clientConnection[10];
      int c_user_num;

      TAesClass *aes = new TAesClass;
      xml * xml_conf = new xml;
      send_file *send = new send_file;
      terminal_cmd *terminal = new terminal_cmd;


      baidu_map * map = new baidu_map;

       void readMessage(QTcpSocket * socket);  //接收数据
       void sendMessage(QTcpSocket * socket, char* s_data);
       void sendFile(struct m_client * p_clinet, QString fileName);
       void do_cmd(QString cmd, QTcpSocket * socket);
       void prase_bd_info( QDomElement rootnode, QTcpSocket * socket);

       void show_server_conf(void);

       QList<struct m_client *> msg_clinet_list;
       QList<struct m_client *> file_clinet_list;


       void client_ui_clear(QString id);
       void log_printf(QString log_text);
       /*Yin版本发布系统*/

       int find_msg_clinet_point(QString id);

        void email_data(char * data, uint8_t addr);
        void show_client(struct m_client * p_clinet );
        int verison;

        /*file*/

        QString get_file_md5(QString str_path);

private slots:

    void on_pushButton_clicked();
    void send_init_message();
     void send_init_file();
    void m_disconnect();
    void readsocket_data();
    void sendFileBody(qint64 numBytes);
    void auto_scanf_down(void);


    void on_pushButton_new_verison_clicked();

    void on_listView_clicked(const QModelIndex &index);

    void on_pushButton_send_file_clicked();

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void replyFinished(QNetworkReply *reply);

    void on_pushButton_send_cmd_clicked();

    void on_change_gid_click(bool);

private:
    Ui::MainWindow *ui;

    QTcpServer *tcpServer,
                *tcp_file_server;

    QByteArray outBlock;    //数据缓冲区，即存放每次要发送的数据


    qint64 loadSize;   //每次发送数据的大小

    QString message;  //存放从服务器接收到的字符串
    quint16 blockSize; //存放文件的大小信息

    //void read_conf(void);

   // void prase_group_conf(QDomElement child_element);
    void init_ui(void);


signals:
    void terminal_bak(QString str);

public slots:
    void send_terminal_cmd(QString str_cmd);

};

#endif // MAINWINDOW_H
