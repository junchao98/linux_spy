#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QModelIndex>
#include <QtGlobal>
#include <QtGlobal>

#include <QFile>
#include <QDirIterator>
#include <QFileDialog>
#include <QMenu>

#include <QMessageBox>
/*
 *1.下线处理
 *
 * */
enum TABLE_POINT{
    CHECK_POINT = 0,
    GROUPID_POINT,
    ID_POINT,
    IP_POINT,
    HD_POINT,
    CPU_POINT,
    TIME_POINT,
    PLACE_POINT,
    VER_POINT

};

enum DOWN_TYPE{

    ST_PACK = 0,
    ST_FILE
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


    map->baidu_map_init();

    /*post 返回信息*/
    connect(map->manager, SIGNAL(finished(QNetworkReply*)),this,SLOT(replyFinished(QNetworkReply*)));

    /*terminal 信号槽*/
    connect(this, SIGNAL(terminal_bak(QString)),terminal,SLOT(get_terminal_bak(QString)));
    connect(terminal, SIGNAL(terminal_cmd_ready(QString)),this,SLOT(send_terminal_cmd(QString)));


    init_ui();

    /*test*/


    /*end test*/


    log_printf("读取配置文件");

#ifdef Q_OS_WIN32

       xml_conf->read_conf("d:/lus_server_conf.xml");

       log_printf("windows system");
#endif


#ifdef Q_OS_LINUX

       xml_conf->read_conf("/home/lornyin/work/lus/lus_server_conf.xml");

       log_printf("linux system");
#endif


    show_server_conf();

    qDebug() << "max_down "<<xml_conf->server_conf->max_down;

    /*下载列表扫描定时器*/
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


    /*加密模块初始化*/
    UCHAR *p = aes_key;
    aes->InitializePrivateKey(16, p); //进行初始化

}

MainWindow::~MainWindow()
{
    delete ui;
}


/*POST 返回信息接收*/
void MainWindow::replyFinished(QNetworkReply *reply)
{
    QByteArray byte_data;
    //QTextCodec *codec = QTextCodec::codecForName("utf8");
    //QString all = codec->toUnicode(reply->readAll());

     byte_data= reply->readAll();

     map->prase_ip_info(byte_data);


}


/*发送shell命令*/
void MainWindow::send_terminal_cmd(QString str_cmd)
{

    qDebug() << "send termin cmd";

    for(int i=0; i<ui->tableWidget->rowCount(); i++){

           if(ui->tableWidget->item(i, CHECK_POINT)->checkState() == Qt::Checked){


                    QString str_id =    ui->tableWidget->item(i, ID_POINT)->text();
                    qDebug() << "int id" << str_id;
                    int id = find_msg_clinet_point(str_id);

                    ui->textBrowser->append(str_cmd);

                    sendMessage( msg_clinet_list.at(id)->clientConnection,
                                 str_cmd.toLatin1().data());

           }

       }

}

void MainWindow::log_printf(QString log_text)
{

    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = time.toString("h:m:s ap  " ); //设置显示格式
    ui->textBrowser->append(str + log_text);

}

void MainWindow::init_ui(void)
{

    ui->tableWidget->setColumnWidth(CHECK_POINT, 40);
    ui->tableWidget->setColumnWidth(GROUPID_POINT, 40);
    ui->tableWidget->setColumnWidth(ID_POINT, 65);
    ui->tableWidget->setColumnWidth(IP_POINT, 100);
    ui->tableWidget->setColumnWidth(HD_POINT, 200);

    QIcon icon;
    icon.addFile(tr(":/res/app_send.ico"));
    ui->pushButton_send_file->setIcon(icon);
    ui->pushButton_send_file->setText("");
    ui->pushButton_send_file->setFlat(true);
    ui->pushButton_send_file->setIconSize(QSize(75, 75));
    ui->pushButton_send_file->setToolTip("发送文件");

    icon.addFile(tr(":/res/app_edit.ico"));
    ui->pushButton_send_cmd->setIcon(icon);
    ui->pushButton_send_cmd->setText("");
    ui->pushButton_send_cmd->setFlat(true);
    ui->pushButton_send_cmd->setIconSize(QSize(75, 75));
    ui->pushButton_send_cmd->setToolTip("命令终端");

    icon.addFile(tr(":/res/app_search.ico"));
    ui->pushButton_serch->setIcon(icon);
    ui->pushButton_serch->setText("");
    ui->pushButton_serch->setFlat(true);
    ui->pushButton_serch->setIconSize(QSize(75, 75));
    ui->pushButton_serch->setToolTip("nop");

    icon.addFile(tr(":/res/app_info.ico"));
    ui->pushButton_top_info->setIcon(icon);
    ui->pushButton_top_info->setText("");
    ui->pushButton_top_info->setFlat(true);
    ui->pushButton_top_info->setIconSize(QSize(75, 75));
     ui->pushButton_top_info->setToolTip("查看设备信息");

     QPixmap image_init;
     image_init.load(":/res/about.png");
     ui->label_about->setPixmap(image_init);

     ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
     ui->tableWidget->setFrameShape(QFrame::NoFrame);
     ui->tableWidget-> setSelectionBehavior(QAbstractItemView::SelectRows);


}

void MainWindow:: show_server_conf(void)
{

    ui->lineEdit_max_down->setText(xml_conf->server_conf->max_down);

     // xml._server_conf ;

   QStandardItemModel * standardItemModel = new QStandardItemModel(this);

        QStringList strList;

        for(int i=0; i < xml_conf->server_conf->verison_inf_list.count(); i++){

            strList.append("组ID:"+ xml_conf->server_conf->verison_inf_list.at(i)->group_id + "  版本号:" + xml_conf->server_conf->verison_inf_list.at(i)->verison);
            ui->comboBox_group->addItem("组ID:"+ xml_conf->server_conf->verison_inf_list.at(i)->group_id);
        }

        /*版本信息list显示*/
        int nCount = strList.size();
        for(int i = 0; i < nCount; i++)
        {
            QString string = static_cast<QString>(strList.at(i));
            QStandardItem *item = new QStandardItem(string);
            if(i % 2 == 1)
            {
                QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
                linearGrad.setColorAt(0, Qt::darkGreen);
                linearGrad.setColorAt(1, Qt::yellow);
                QBrush brush(linearGrad);
                item->setBackground(brush);
            }
            standardItemModel->appendRow(item);
        }
        ui->listView->setModel(standardItemModel);
        ui->listView->setFixedSize(200,300);


}


void MainWindow::auto_scanf_down(void)
{

     /*@添加最大下载数限定*/

    /*没有等待下载的client*/
    if(file_clinet_list.size() == 0)return;
	
    for(int i=0; i<file_clinet_list.size(); i++){

        qDebug()<<"file_inf.down_status: "<<file_clinet_list.at(i)->down_info.type;

        if(file_clinet_list.at(i)->down_info.status == 1){

            qDebug()<<"send file";

            sendFile(file_clinet_list.at(i), file_clinet_list.at(i)->down_info.file_path);

            file_clinet_list.at(i)->down_info.status  = 0;

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


   //qDebug()<< "file size :" <<  p_clinet->file_inf.totalBytes;

    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);

    QString currentFileName;

    if(fileName.indexOf('/') != -1){

         currentFileName = fileName.right(fileName.size()- fileName.lastIndexOf('/')-1);

    }else if(fileName.indexOf('\\') != -1){

            qDebug() << "lastIndexOf" << fileName.lastIndexOf('\\');
         currentFileName = fileName.right(fileName.size()- fileName.lastIndexOf('\\')-1);

    }

    qDebug() << "currentFileName " << currentFileName;
    //依次写入文件类型、总大小信息空间、文件名大小信息空间、文件名
    sendOut << qint64(0)<<qint64(0) << qint64(0) << currentFileName;

    //这里的总大小是文件名大小等信息和实际文件大小的总和
    p_clinet->file_inf.totalBytes += outBlock.size();

    sendOut.device()->seek(0);


    //返回outBolock的开始，用实际的大小信息代替3个qint64(0)空间
    sendOut<<p_clinet->down_info.type <<p_clinet->file_inf.totalBytes <<qint64((outBlock.size() - sizeof(qint64)*3));

    qDebug() << p_clinet->down_info.type <<" "<<p_clinet->file_inf.totalBytes << qint64((outBlock.size() - sizeof(qint64)*3));

    //发送完头数据后剩余数据的大小
    p_clinet->file_inf.bytesToWrite = p_clinet->file_inf.totalBytes - p_clinet->clientConnection->write(outBlock);

    outBlock.resize(0);
}


//实现文件的传送 自动调用
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

        /*发送动态ID到client*/
        msg_str.sprintf("<init_msg id=\"%x\" now_verison=\"\"></init_msg>", p_clinet->clientConnection);
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


    p_clinet->clientConnection = tcp_file_server->nextPendingConnection();

    //connect(p_clinet->clientConnection,SIGNAL(readyRead()),this,SLOT(readsocket_data()));

    connect(p_clinet->clientConnection,SIGNAL(disconnected()),p_clinet->clientConnection,SLOT(deleteLater()));
    connect(p_clinet->clientConnection,SIGNAL(bytesWritten(qint64)),this,SLOT(sendFileBody(qint64)));

    qDebug("file connect %x ", p_clinet->clientConnection);


     /*a new device online*/
    if( p_clinet->clientConnection != NULL){

        if(down_info.status){

            p_clinet->down_info = down_info;
            down_info.status = 0;

        }else{

            qDebug() << " down_info is unavailable";
            return;
        }


        file_clinet_list.append(p_clinet);

    }else{

         ui->textBrowser->append("CONNECT ERROE");

    }



}

/*tablewidget 创建client 并显示IP，物理地址*/
void MainWindow::show_client(struct m_client * p_clinet )
{

    QString inf_str;
	QString ip_str;

    inf_str.sprintf("%x", p_clinet->clientConnection);

    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, ID_POINT, new QTableWidgetItem(inf_str));

    QTableWidgetItem *check_box = new QTableWidgetItem();
    check_box->setCheckState(Qt::Unchecked);
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, CHECK_POINT, check_box);

    QDateTime time = QDateTime::currentDateTime();
    QString time_str = time.toString("hh:m:s");
    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, TIME_POINT, new QTableWidgetItem(time_str));

	ip_str = p_clinet->clientConnection->peerAddress().toString();

    ip_str = ip_str.right(ip_str.size()-7);
    map->post_ip(ip_str);

    ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, IP_POINT, new QTableWidgetItem(ip_str));

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
   // qDebug() << "check " << strlen(jiemi) << jiemi[strlen(jiemi) -1];

    qDebug() << "recive: " << QString(QLatin1String(jiemi));

    do_cmd(QString(QLatin1String(jiemi)), socket);


}

/*解析msg 的xml语句*/
void MainWindow::do_cmd(QString cmd, QTcpSocket * socket)
{

   QDomDocument xml;
   xml.setContent(cmd);

   QDomElement rootnode = xml.documentElement();

   if(rootnode.tagName() == "bd_info"){

       prase_bd_info(rootnode, socket);

   }


   if(rootnode.tagName() == "requst"){



       if(rootnode.attributeNode("type").value() == "kernel_down"){

            down_info.type = ST_PACK;

            int point = find_msg_clinet_point(rootnode.attributeNode("id").value());

            if(point == -1){

                qDebug()<<"not found clinet";
                return;
            }


            for(int i=0; i<xml_conf-> server_conf->verison_inf_list.size(); i++){

                if(xml_conf-> server_conf->verison_inf_list.at(i)->group_id == msg_clinet_list.at(point)->group_id){

                    down_info.file_path = xml_conf-> server_conf->verison_inf_list.at(i)->file_path;

                    qDebug() << "file_path"<<down_info.file_path;
                    down_info.status = 1;


                    sendMessage(msg_clinet_list.at(point)->clientConnection,  "<send_file type=\"kernel\" md5=\"\" />");
                }

            }

       }

   }


   if(rootnode.tagName() == "term_cmd_bak"){

        QString str = rootnode.attributeNode("body").value();
        emit terminal_bak(str);

   }

}

/*解析这段状态xml 并在ui显示*/
void MainWindow::prase_bd_info( QDomElement rootnode, QTcpSocket * socket)
{

    QString cpu_info,
            hd_info,
            client_id,
            group_id;

    client_id = rootnode.attributeNode("id").value();
    cpu_info  = rootnode.attributeNode("cpu_info").value();
    hd_info   = rootnode.attributeNode("disk_space").value();
    group_id  = rootnode.attributeNode("group_id").value();

    qDebug() << "prase_bd_info";
    for(int i=0; i<msg_clinet_list.size(); i++){

        if(msg_clinet_list.at(i)->clientConnection == socket){

            msg_clinet_list.at(i)->group_id = rootnode.attributeNode("group_id").value();
            qDebug()<<"get group id" << msg_clinet_list.at(i)->group_id;
            break;
        }

    }


    ui->textBrowser->append(cpu_info + "  " + hd_info);

    for(int i=0; i<ui->tableWidget->rowCount(); i++){

        if(ui->tableWidget->item(i, ID_POINT)->text() == client_id){

            ui->tableWidget->setItem(i, HD_POINT, new QTableWidgetItem(hd_info));
            ui->tableWidget->setItem(i, CPU_POINT, new QTableWidgetItem(cpu_info));
            ui->tableWidget->setItem(i, GROUPID_POINT, new QTableWidgetItem(group_id));


        }
    }

    /*根据组ID 发送对应版本最新*/

    int new_verison = -1;
    for(int i=0; i<xml_conf->server_conf->verison_inf_list.size(); i++){

        if(xml_conf->server_conf->verison_inf_list.at(i)->group_id== group_id){

            new_verison = xml_conf->server_conf->verison_inf_list.at(i)->verison.toInt();
            break;

        }

        qDebug()<<"未找到此组ID";

        return;
    }

    QString msg_str;
    msg_str.sprintf("<verison_info id=\"%x\" now_verison=\"%d\"></init_msg>", socket, new_verison);
    QByteArray ba = msg_str.toLatin1();
    sendMessage(socket, ba.data());

}

int MainWindow::find_msg_clinet_point(QString id)
{
    for(int i=0; i<msg_clinet_list.size(); i++){

        if(msg_clinet_list.at(i)->id == id)return i;
    }

    return -1;
}

/*弃用*/
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
                msg_str.sprintf("%x", clientConnection);
                ui->textBrowser->append(msg_str+" disconnected");

              msg_clinet_list.removeAt(i);

              clientConnection->deleteLater();

              client_ui_clear(msg_str);
             /*@添加ui 清理工作*/
             break;

        }

     }


 }

/*从table删除disconnected 的 client*/
void MainWindow::client_ui_clear(QString id)
{

    for(int i=0; i<ui->tableWidget->rowCount(); i++){


         qDebug() << ui->tableWidget->item(i, ID_POINT)->text();
            if(ui->tableWidget->item(i, ID_POINT)->text() == id){
                ui->tableWidget->removeRow(i);
                qDebug() << "found clear";

            }
    }

}



/*发布新的版本信息*/
void MainWindow::on_pushButton_new_verison_clicked()
{

    if(ui->lineEdit_verison_num->text().isEmpty()){

            QMessageBox::information(this,"错误","请输入版本号");
            return ;
    }

    if(ui->lineEdit_group_id->text().isEmpty()){

            QMessageBox::information(this,"错误","请输入组ID");
            return ;
    }


   QString path=QFileDialog::getOpenFileName(this,"选择文件","./Phone","update(*.tar.bz2);;tel(*.sh)");

   xml_conf->v_inf->file_path = path;
   xml_conf->v_inf->group_id = ui->lineEdit_group_id->text();
   xml_conf->v_inf->verison = ui->lineEdit_verison_num->text();


   xml_conf->add_new_verison();


}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{

    QString str =  ui->listView->model()->index(index.row(), 0).data().toString();

    str = str.left(str.indexOf(" "));

    str = str.right(str.size() - str.indexOf(":") - 1);

    ui->lineEdit_group_id->setText(str);
}

void MainWindow::on_pushButton_send_file_clicked()
{

    QString path=QFileDialog::getOpenFileName(this,"选择文件","./Phone","update(*.tar.bz2);;tel(*.*)");

    send->exec();

    qDebug() << path;

    if(path.isEmpty())return;

    QString cmd;

    cmd =  "<send_file ";
    cmd += " type=\"" + send->file_mark.str_file_type + "\"";

    if(send->file_mark.enabled_md5){

         cmd += " md5=\""+  get_file_md5(path)+ "\">";

    }else{

         cmd += " md5=\"\">";

    }

    qDebug() << cmd;

    for(int i=0; i<ui->tableWidget->rowCount(); i++){

           if(ui->tableWidget->item(i, CHECK_POINT)->checkState() == Qt::Checked){


                    QString str_id =    ui->tableWidget->item(i, ID_POINT)->text();
                    qDebug() << "int id" << str_id;
                    int id = find_msg_clinet_point(str_id);

                    down_info.file_path = path;
                    down_info.type = send->file_mark.file_type;
                    down_info.status = 1;

                    ui->textBrowser->append(cmd);

                    sendMessage( msg_clinet_list.at(id)->clientConnection, cmd.toLatin1().data());

           }


    }

}


QString MainWindow::get_file_md5(QString str_path)
 {


     QFile theFile(str_path);
     theFile.open(QIODevice::ReadOnly);
     QByteArray ba = QCryptographicHash::hash(theFile.readAll(), QCryptographicHash::Md5);
     theFile.close();
    return ba.toHex().constData();

 }


void MainWindow::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{

    QMenu *cmenu = new QMenu(ui->tableWidget);
    QAction *action1_change_gid = cmenu->addAction("修改组ID");
    QAction *action2 = cmenu->addAction("Menu 2");
    QAction *action3 = cmenu->addAction("Menu 3");
    connect(action1_change_gid, SIGNAL(triggered(bool)), this, SLOT(on_change_gid_click(bool)));
   // connect(action2, SIGNAL(triggered(bool)), this, SLOT(on_menu_click(bool)));
   // connect(action3, SIGNAL(triggered(bool)), this, SLOT(on_menu_click(bool)));
    cmenu->exec(QCursor::pos());


}

void MainWindow::on_pushButton_send_cmd_clicked()
{

    terminal->show();



}


void MainWindow::on_change_gid_click(bool)
{


    QString cmd = "<cmd body=\"change_gid\" para1=\"xxxx\" />";
    QString id;
    int point;

    qDebug() << "on_change_gid_click";

    c_gid->exec();

    if(c_gid->new_gid.isEmpty())return;

    for(int i=0; i<ui->tableWidget->rowCount(); i++){

        if(ui->tableWidget->item(i,ID_POINT)->isSelected()){

              id = ui->tableWidget->item(i, ID_POINT)->text();
              point =  find_msg_clinet_point(id);

              cmd.replace(cmd.indexOf("xxxx"), 4, c_gid->new_gid);

              sendMessage(msg_clinet_list.at(point)->clientConnection, cmd.toLatin1().data());

              qDebug() << cmd;
        }

    }





    c_gid->new_gid.clear();
}




















