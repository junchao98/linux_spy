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

void xml::add_new_verison(void)
{
     QDomDocument document;

    QFile file(xml_path);
       if(!file.open(QFile::ReadWrite | QFile::Text))
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

     QDomNode child_node;
     QDomElement child_element;
     int i=0;

   /*枚举读取子节点*/
   for(i=0; i< child_list.count(); i++){

         child_node = child_list.item(i);
         child_element = child_node.toElement();

        if(child_element.tagName() == "group_conf")break;

   }


    if(i != child_list.count()){


        QDomNodeList group_child_list = child_element.childNodes();
        int j=0;
        /*枚举ID是否存在*/
        for(j=0; j< group_child_list.count(); j++){     //修改版本信息

             QDomNode child_node = group_child_list.item(j);
             QDomElement child_element = child_node.toElement();

               if( child_element.attributeNode("id").value() == v_inf->group_id){
                   /*组ID已存在*/
                   child_element.attributeNode("verison").setValue(v_inf->verison);
                   child_element.attributeNode("path").setValue(v_inf->file_path);
                    break;

               }


        }//end for

       if(j == group_child_list.count()){   //新建组、新建版本信息

           qDebug() << "add a new group";

           QDomElement	new_element = child_node.toDocument().createElement("group");


            new_element.setAttribute("id", v_inf->group_id);
            new_element.setAttribute("verison", v_inf->verison);
            new_element.setAttribute("path", v_inf->file_path);

            //child_node.toDocument().appendChild(new_element);
            child_node.appendChild(new_element);

       }


    }//end if


    /*保存更新后的XML文件*/

    file.seek(0);
   QTextStream out(&file);
   out.reset();
   out.setCodec("utf-8");
   document.save(out, 4, QDomNode::EncodingFromTextStream);

   file.close();
}



















