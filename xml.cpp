#include "xml.h"


void  xml::read_conf(QString path)
 {

    server_conf = new struct _server_conf;
    QDomDocument document;
    xml_path = path;

    QFile file(path);
       if(!file.open(QFile::ReadOnly | QFile::Text))
       {
          qDebug() << "open error";
           return ;
       }


    QString error;
    int row = 0, column = 0;
    if(!document.setContent(&file, false, &error, &row, &column))
    {
        qDebug() << "prase error";

             return ;
     }


    /*读取root节点*/
    QDomElement root_node = document.documentElement();

    qDebug() << root_node.tagName();

    //client_verison = root_node.attributeNode("varison").value();

    QDomNodeList child_list = root_node.childNodes();
    qDebug()<< "child nodes = "<<child_list.count();

   /*枚举读取子节点*/
   for(int i=0; i< child_list.count(); i++){

        QDomNode child_node = child_list.item(i);
        QDomElement child_element = child_node.toElement();

        if(child_element.tagName() == "group_conf")prase_group_conf(child_element);
        if(child_element.tagName() == "down_conf")prase_down_conf(child_element);

   }


    file.close();

 }



/*读取每组的版本信息*/
void xml::prase_group_conf(QDomElement child_element)
{


    QDomNodeList group_child_list = child_element.childNodes();

    /*枚举读取子节点*/
    for(int i=0; i< group_child_list.count(); i++){

         QDomNode child_node = group_child_list.item(i);
         QDomElement child_element = child_node.toElement();
         struct _verison_inf *info = new struct _verison_inf;

          info->group_id = child_element.attributeNode("id").value();

          qDebug()<<child_element.attributeNode("id").value();

          info->verison = child_element.attributeNode("verison").value();
          qDebug()<<child_element.attributeNode("verison").value();

          server_conf->verison_inf_list.append(info);


    }


}


void xml::prase_down_conf(QDomElement child_element)
{

    server_conf->max_down = child_element.attributeNode("max_down").value();

}

void xml::add_new_verison( struct _verison_inf *v_inf)
{
     QDomDocument document;

    QFile file(xml_path);
       if(!file.open(QFile::ReadOnly | QFile::Text))
       {
          qDebug() << "open error";
           return ;
       }

    QString error;
    int row = 0, column = 0;
    if(!document.setContent(&file, false, &error, &row, &column))
    {
        qDebug() << "prase error";
             return ;
    }


    /*读取root节点*/
    QDomElement root_node = document.documentElement();

    if(root_node.tagName() != "lus_server"){

        qDebug()<<"error xml file";
    }

    QDomNodeList child_list = root_node.childNodes();

   /*枚举读取子节点*/
   for(int i=0; i< child_list.count(); i++){

        QDomNode child_node = child_list.item(i);
        QDomElement child_element = child_node.toElement();

        if(child_element.tagName() == "group_conf")chang_group_conf(child_element);

   }



}

void xml::chang_group_conf(QDomElement child_element, struct _verison_inf *v_inf)
{


    QDomNodeList group_child_list = child_element.childNodes();

    /*枚举ID是否存在*/
    for(int i=0; i< group_child_list.count(); i++){

         QDomNode child_node = group_child_list.item(i);
         QDomElement child_element = child_node.toElement();
         struct _verison_inf *info = new struct _verison_inf;

          info->group_id = child_element.attributeNode("id").value();

          qDebug()<<child_element.attributeNode("id").value();

          info->verison = child_element.attributeNode("verison").value();
          qDebug()<<child_element.attributeNode("verison").value();

          server_conf->verison_inf_list.append(info);


    }


}

