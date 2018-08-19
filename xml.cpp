#include "xml.h"


void*  xml::read_conf(QString path)
 {

     server_conf = (struct _server_conf *)malloc(sizeof(struct _server_conf)) ;

    QFile file(path);
       if(!file.open(QFile::ReadOnly | QFile::Text))
       {
          qDebug() << "open error";
           return NULL;
       }

    QDomDocument document;
    QString error;
    int row = 0, column = 0;
    if(!document.setContent(&file, false, &error, &row, &column))
    {
        qDebug() << "prase error";

             return NULL;
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

    return server_conf;

 }



/*读取每组的版本信息*/
void xml::prase_group_conf(QDomElement child_element)
{

   struct _verison_inf *info = new struct _verison_inf;
    QDomNodeList group_child_list = child_element.childNodes();

    /*枚举读取子节点*/
    for(int i=0; i< group_child_list.count(); i++){

         QDomNode child_node = group_child_list.item(i);
         QDomElement child_element = child_node.toElement();

          info->group_id = child_element.attributeNode("id").value();

          //qDebug()<<child_element.attributeNode("id").value();

          info->verison = child_element.attributeNode("verison").value();
          //qDebug()<<child_element.attributeNode("verison").value();

          server_conf->verison_inf_list.append(info);

    }


}


void xml::prase_down_conf(QDomElement child_element)
{

    server_conf->max_down = child_element.attributeNode("down_conf").value();


}


