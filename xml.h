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

    void*  read_conf(QString path);


private:

    void prase_group_conf(QDomElement child_element);
    void prase_down_conf(QDomElement child_element);

};

#endif // XML_H
