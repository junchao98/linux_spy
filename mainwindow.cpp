#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>


enum TABLE_POINT{
    CHECK_POINT = 0,
    GROUPID_POINT,
    ID_POINT,
    IP_POINT,
    HD_POINT,
    CPU_POINT,
    TIME_POINT,
    VER_POINT

};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    c_user_num = 0;
    blockSize = 0;
    verison = 0;
    loadSize = 4*1024;

    ui->tableWidget->setColumnWidth(CHECK_POINT, 40);
    ui->tableWidget->setColumnWidth(ID_POINT, 55);
    ui->tableWidget->setColumnWidth(IP_POINT, 100);
    ui->tableWidget->setColumnWidth(HD_POINT, 200);


    xml_conf->read_conf("d:/xml.conf");

    qDebug() << "max_down "<<xml_conf->server_conf->max_down;

     timer_down = new QTimer(this);
     connect( timer_down, SIGNAL( timeout() ), this, SLOT( auto_scanf_down() ) );
     timer_down->start(5000);   //5s


    /*数据通信链接*/
    tcpServer = new QTcpServer(this);
    if(!tcpServer->listen(QHostAddress::Any,9527))
    {  //**本地主机的6666端口，如果出错就输出错误信息，并关闭
        qDebug() << tcpServer->errorString();
        close();
    }

    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(send_init_message()));

    /*文件传输链接*/
    tcp_file_server = new QTcpServer(this);

    if(!tcp_file_server->listen(QHostAddress::Any,9528))
    {  //**本地主机的6666端口，如果出错就输出错误信息，并关闭
        qDebug() << tcp_file_server->errorString();
        close();
    }

    connect(tcp_file_server,SIGNAL(newConnection()),this,SLOT(send_init_file()));


    UCHAR *p = aes_key;
    aes->InitializePrivateKey(16, p); //进行初始化

}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::auto_scanf_down(void)
{

    /*没有等待下载的client*/
    if(file_clinet_list.size() == 0)return;

    for(int i=0; i<file_clinet_list.size(); i++){

        if(file_clinet_list.at(i)->file_inf.down_status){
            sendFile(file_clinet_list.at(i), "/home/lornyin/work/lus/update.tar.bz2");
            file_clinet_list.at(i)->file_inf.down_status = false;

        }


    }


}



void MainWindow::on_pushButton_clicked()
{



    QString tmp;
    tmp = ui->lineEdit->text();
    QByteArray ba = tmp.toLatin1();
    char *p_tmp = ba.data();

    for(int i=0; i<msg_clinet_list.size(); i++){

       sendMessage( msg_clinet_list.at(i)->clientConnection, p_tmp);

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

     //size = 15;
     if(size > 1024)return;

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

    socket->flush();

}


void MainWindow::sendFile(struct m_client * p_clinet, QString fileName)  //实现文件大小等信息的发送
{

    p_clinet->file_inf.localFile = new QFile(fileName);

    QFile * file =  p_clinet->file_inf.localFile;

    if(! file->open(QFile::ReadOnly))
    {
       qDebug() << "open file error!";
       return;
    }

    //文件总大小
   p_clinet->file_inf.totalBytes = file->size();


   qDebug()<< "file size :" <<  p_clinet->file_inf.totalBytes;

    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);
    QString currentFileName = fileName.right(fileName.size()- fileName.lastIndexOf('/')-1);

    //依次写入总大小信息空间，文件名大小信息空间，文件名
    sendOut << qint64(0) << qint64(0) << currentFileName;

    //这里的总大小是文件名大小等信息和实际文件大小的总和
    p_clinet->file_inf.totalBytes += outBlock.size();

    sendOut.device()->seek(0);
    //返回outBolock的开始，用实际的大小信息代替两个qint64(0)空间
    sendOut<<p_clinet->file_inf.totalBytes<<qint64((outBlock.size() - sizeof(qint64)*2));

    //发送完头数据后剩余数据的大小
    p_clinet->file_inf.bytesToWrite = p_clinet->file_inf.totalBytes - p_clinet->clientConnection->write(outBlock);

    outBlock.resize(0);
}


//更新进度条，实现文件的传送
void MainWindow::sendFileBody(qint64 numBytes)
{

    struct m_client * p_clinet;
    QTcpSocket * clientConnection = qobject_cast<QTcpSocket *>(sender());

    /*查找、私有化操作的client*/
    for(int i=0; i<file_clinet_list.size(); i++){

        if(file_clinet_list.at(i)->clientConnection == clientConnection){

            p_clinet=file_clinet_list.at(i);
            qDebug() << "found clinet ";
        }

    }
    //已经发送数据的大小
    p_clinet->file_inf.bytesWritten += (int)numBytes;

    //qDebug() << "bytesWritten="<< p_clinet->file_inf.bytesWritten;

    if(p_clinet->file_inf.bytesToWrite > 0) //如果已经发送了数据
    {
   //每次发送loadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，
   //就发送剩余数据的大小
       outBlock =p_clinet->file_inf.localFile->read(qMin(p_clinet->file_inf.bytesToWrite,loadSize));

       //发送完一次数据后还剩余数据的大小
       p_clinet->file_inf.bytesToWrite -= (int)clientConnection->write(outBlock);

       //清空发送缓冲区
       outBlock.resize(0);

    } else {
       p_clinet->file_inf.localFile->close(); //如果没有发送任何数据，则关闭文件
    }

    //更新进度条
   // ui->clientProgressBar->setMaximum(totalBytes);
   // ui->clientProgressBar->setValue(bytesWritten);

    if(p_clinet->file_inf.bytesWritten == p_clinet->file_inf.totalBytes) //发送完毕
    {
       //ui->textBrowser->append("传送文件成功");
       p_clinet->file_inf.localFile->close();
       p_clinet->clientConnection->close();
       qDebug() << "传送文件成功";
       file_clinet_list.removeAt(file_clinet_list.indexOf(p_clinet));
    }
}



void MainWindow::send_init_message()
{
    //我们获取已经建立的连接的子套接字

    struct m_client * p_clinet = new  struct m_client;

    p_clinet->clientConnection = tcpServer->nextPendingConnection();

    connect(p_clinet->clientConnection,SIGNAL(readyRead()),this,SLOT(readsocket_data()));

    //connect(p_clinet->clientConnection,SIGNAL(disconnected()),p_clinet->clientConnection,SLOT(deleteLater()));

    connect(p_clinet->clientConnection,SIGNAL(disconnected()),this,SLOT( m_disconnect()));

    qDebug("connect %x ", p_clinet->clientConnection);


     /*a new device online*/
    if( p_clinet->clientConnection != NULL){


        //QString msg_str =QString("%x").arg(p_clinet->clientConnection);
        QString msg_str;
        msg_str.sprintf("%x CONNECT", p_clinet->clientConnection);
        ui->textBrowser->append(msg_str);
        msg_str.clear();

        msg_str.sprintf("%x",  p_clinet->clientConnection);
        p_clinet->id = msg_str;
        msg_str.clear();

        msg_str.sprintf("<init_msg id=\"%x\" now_verison=\"%d\"></init_msg>", p_clinet->clientConnection, 2);
        QByteArray ba = msg_str.toLatin1();
        sendMessage(p_clinet->clientConnection, ba.data());

        c_user_num++;
        ui->label_user_num->setText(QString::number(c_user_num, 10));

        msg_clinet_list.append(p_clinet);

        show_client(p_clinet);

    }else{

         ui->textBrowser->append("CONNECT ERROE");

    }



}


void MainWindow::send_init_file()
{
    //我们获取已经建立的连接的子套接字

    struct m_client * p_clinet = new  struct m_client;

    p_clinet->file_inf.bytesToWrite = 0;
    p_clinet->file_inf.bytesWritten = 0;
    p_clinet->file_inf.totalBytes = 0;
    p_clinet->file_inf.down_status = true;

    p_clinet->clientConnection = tcp_file_server->nextPendingConnection();

    //connect(p_clinet->clientConnection,SIGNAL(readyRead()),this,SLOT(readsocket_data()));

    connect(p_clinet->clientConnection,SIGNAL(disconnected()),p_clinet->clientConnection,SLOT(deleteLater()));
    connect(p_clinet->clientConnection,SIGNAL(bytesWritten(qint64)),this,SLOT(sendFileBody(qint64)));

    qDebug("file connect %x ", p_clinet->clientConnection);


     /*a new device online*/
    if( p_clinet->clientConnection != NULL){

        //sendFile(p_clinet, "d://test.jpg");

        file_clinet_list.append(p_clinet);

        //show_client(p_clinet);

    }else{

         ui->textBrowser->append("CONNECT ERROE");

    }



}






void MainWindow::show_client(struct m_client * p_clinet )
{

    QString inf_str;

    inf_str.sprintf("%x", p_clinet->clientConnection);



    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ID_POINT, new QTableWidgetItem(inf_str));

    QTableWidgetItem *check_box = new QTableWidgetItem();
    check_box->setCheckState(Qt::Unchecked);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, CHECK_POINT, check_box);

    QDateTime time = QDateTime::currentDateTime();
    QString time_str = time.toString("hh:m:s");
     ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, TIME_POINT, new QTableWidgetItem(time_str));


    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, IP_POINT, new QTableWidgetItem(p_clinet->clientConnection->peerAddress().toString()));


}

void MainWindow::readsocket_data()
{

    QTcpSocket * clientConnection = qobject_cast<QTcpSocket *>(sender());

    readMessage(clientConnection);

}

void MainWindow::readMessage(QTcpSocket * socket)
{

    QDataStream in(socket);
    char data[1024] = {0};


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

    //for(int i=0; i<32; i++) qDebug("# %x", data[i]);

    qDebug()<<"un size" << (DWORD)sizeof(char)*(blockSize-(int)sizeof(quint16));


    socket->flush();

    blockSize = 0;

    ui->textBrowser->append(QString(QLatin1String(jiemi)));

   //addr =  get_data_toaddr(jiemi);
  // email_data(jiemi, addr);
    qDebug() << "check " << strlen(jiemi) << jiemi[strlen(jiemi) -1];


    do_cmd(QString(QLatin1String(jiemi)));


}


void MainWindow::do_cmd(QString cmd)
{

   QDomDocument xml;
   xml.setContent(cmd);

   QDomElement rootnode = xml.documentElement();

   if(rootnode.tagName() == "bd_info"){

       prase_bd_info(rootnode);

   }


}


void MainWindow::prase_bd_info( QDomElement rootnode)
{

    QString cpu_info,
            hd_info,
            client_id;

    client_id = rootnode.attributeNode("id").value();
    cpu_info  = rootnode.attributeNode("cpu_info").value();
    hd_info   = rootnode.attributeNode("disk_space").value();

    ui->textBrowser->append(cpu_info + "  " + hd_info);

    for(int i=0; i<ui->tableWidget->rowCount(); i++){

        if(ui->tableWidget->item(i, ID_POINT)->text() == client_id){

            ui->tableWidget->setItem(i, HD_POINT, new QTableWidgetItem(hd_info));
            ui->tableWidget->setItem(i, CPU_POINT, new QTableWidgetItem(cpu_info));

        }
    }



}

int MainWindow::find_msg_clinet_point(QString id)
{
    for(int i=0; i<msg_clinet_list.size(); i++){

        if(msg_clinet_list.at(i)->id == id)return i;
    }

    return -1;
}

void MainWindow::email_data(char * data, uint8_t addr)
{


    /*广播消息*/
    if(addr == BROADCAST_ADDR){

        for(int i=0; i<msg_clinet_list.size(); i++){

            sendMessage(msg_clinet_list.at(i)->clientConnection, data);

        }

    }else{
        /*发送给指定clinet*/
        for(int i=0; i<msg_clinet_list.size(); i++){

            //if(msg_clinet_list.at(i)->id == addr){

               // sendMessage(msg_clinet_list.at(i)->clientConnection, data);

           // }

        }


    }


}

void MainWindow::m_disconnect()
 {


   QTcpSocket * clientConnection = qobject_cast<QTcpSocket *>(sender());


    qDebug("some one disconnect");

    c_user_num--;
     ui->label_user_num->setText(QString::number(c_user_num, 10));

     /*find which one disconnect*/
     for(int i=0; i<msg_clinet_list.size(); i++){

        if(msg_clinet_list.at(i)->clientConnection == clientConnection){


                QString msg_str;
                msg_str.sprintf("%x disconnected", clientConnection);
              ui->textBrowser->append(msg_str);

             msg_clinet_list.removeAt(i);
             break;

        }

     }


 }









