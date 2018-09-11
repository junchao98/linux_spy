#ifndef BAIDU_MAP_H
#define BAIDU_MAP_H

#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>

#include <QtNetwork>


class baidu_map
{


public:


    void baidu_map_init(void);

	void prase_ip_info(QByteArray data);
    void post_ip(QString ip);

      QNetworkAccessManager *manager;



private slots:



private:


};

#endif // BAIDU_MAP_H
