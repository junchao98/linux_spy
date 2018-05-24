#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    //aes_key[16]={0x22, 0xa2, 0x13, 0xe2, 0x22, 0xa2, 0x12, 0x20, 0x22, 0x15, 0x22, 0xda, 0x67, 0x22, 0x32, 0x22 };

    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,9527))
    {  //**本地主机的6666端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }

    c_user_num = 0;

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(send_init_message()));

    UCHAR *p = aes_key;
    aes->InitializePrivateKey(16, p); //进行初始化

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{

    //sendMessage("server");

    struct born_order order;
    char * ok_data;
    QString send;

    order_init(&order);

    order.data = (char *)order_list[E_UP];

    ok_data = generdate_data(&order);


    send = QString(QLatin1String(ok_data));

    for(int i =0; i<15; i++)qDebug("%d ", ok_data[i]);

    // qDebug("add = %x ", clientConnection[0] );
   // sendMessage(clientConnection[0], ok_data);


    for(int i=0; i<clinet_list.size(); i++){


       sendMessage( clinet_list.at(i)->clientConnection, "list msg test");

    }

}


void MainWindow::sendMessage(QTcpSocket * socket, char* s_data)
{
    //用于暂存我们要发送的数据
    QByteArray block;

    quint16 en_len = 0;

    //使用数据流写入数据
    QDataStream out(&block,QIODevice::WriteOnly);

     char *send_data = s_data;
     char miwen[1024] = {0};


     //QByteArray ba = s_data.toLatin1(); // must
     //send_data=ba.data();

      qDebug()<<"data" << send_data;

     DWORD size = strlen(send_data);

     size = 15;


      qDebug()<<"enc size "<<size;

     en_len = aes->OnAesEncrypt((LPVOID)send_data, (DWORD)size, (LPVOID)miwen); //进行加密


     //打印信息
    qDebug()<<"en_len "<<en_len;


    //设置数据流的版本，客户端和服务器端使用的版本要相同
    out.setVersion(QDataStream::Qt_5_7);

    out<<(quint16) 0;
    out.writeRawData(miwen, sizeof(char)*en_len);       //原始写！！
    out.device()->seek(0);
    out<<(quint16) (en_len + sizeof(quint16));


    qDebug()<<(quint16) (en_len + sizeof(quint16));

    socket->write(block);

    //发送数据成功后，显示提示
    ui->label->setText("send message successful!!!");

    socket->flush();

}


void MainWindow::send_init_message()
{
    //我们获取已经建立的连接的子套接字

    struct m_client * p_clinet = new  struct m_client;

    p_clinet->clientConnection = tcpServer->nextPendingConnection();

    connect(p_clinet->clientConnection,SIGNAL(readyRead()),this,SLOT(readMessage()));

    connect(p_clinet->clientConnection,SIGNAL(disconnected()),p_clinet->clientConnection,SLOT(deleteLater()));

     blockSize = 0;

    if( p_clinet->clientConnection != NULL){

        ui->textBrowser->append("CONNECT");
        sendMessage(p_clinet->clientConnection, "connect success");
        c_user_num++;
        ui->label_user_num->setText(QString::number(c_user_num, 10));


    }else{

         ui->textBrowser->append("CONNECT ERROE");

    }

clinet_list.append(p_clinet);

}



void MainWindow::readMessage(QTcpSocket * socket)
{

    QDataStream in(socket);
    char data[100] = {0};

    in.setVersion(QDataStream::Qt_5_7);


    //设置数据流版本，这里要和服务器端相同
    in.setVersion(QDataStream::Qt_5_7);

    if(blockSize==0) //如果是刚开始接收数据
    {
       //判断接收的数据是否有两字节，也就是文件的大小信息
       //如果有则保存到blockSize变量中，没有则返回，继续接收数据
       if(socket->bytesAvailable() < (int)sizeof(quint16)) {

           ui->textBrowser->append("no data");
           return;
       }
           in >> blockSize;
    }

    if(socket->bytesAvailable() < blockSize-2) return;
    //如果没有得到全部的数据，则返回，继续接收数据

    in.readRawData(data, blockSize);    //原始读取！

    qDebug()<<"blocksize" <<blockSize;


    //将接收到的数据存放到变量中

    char jiemi[1024] = {0};

    //解密
    aes->OnAesUncrypt((LPVOID)data, (DWORD)sizeof(char)*(blockSize-(int)sizeof(quint16)),(LPVOID)jiemi); //进行解密

    for(int i=0; i<32; i++) qDebug("# %x", data[i]);

    qDebug()<<"un size" << (DWORD)sizeof(char)*(blockSize-(int)sizeof(quint16));

    ui->textBrowser->append(jiemi);

    socket->flush();

    blockSize = 0;

}













