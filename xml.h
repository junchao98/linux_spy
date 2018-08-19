#ifndef XML_H
#define XML_H

#include <qlist.h>
#include <QtXml>
#include <QtXml/QDomDocument>


class xml
{


public:

    struct _verison_inf{

        QString group_id;
        QString file_path;
        QString verison;

    };



    struct _server_conf{

        QList<struct _verison_inf *> verison_inf_list;
        QString max_down;

    };

    struct _server_conf *server_conf;
    struct _verison_inf *v_inf = new struct _verison_inf;

   void  read_conf(QString path);
   void  add_new_verison(void);

private:

    void prase_group_conf(QDomElement child_element);
    void prase_down_conf(QDomElement child_element);

    QString xml_path;


};

#endif // XML_H
