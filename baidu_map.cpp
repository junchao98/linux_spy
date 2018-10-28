#include "baidu_map.h"





void baidu_map::baidu_map_init(void)
{


    manager = new QNetworkAccessManager();

}



void baidu_map::post_ip(QString ip)
{

    QString token_str;
    g_addr.clear();

    QNetworkRequest *request = new QNetworkRequest();

    request->setUrl(QUrl("http://api.map.baidu.com/location/ip"));

    QByteArray postData;

    token_str = "ip="+ip;
    token_str +="&ak=yvR4WBKXzCO8K9yN9TdgdYGHMsKBo94i";

    postData.append(token_str);

    manager->post(*request, postData);

    free(request);

}



void baidu_map::prase_ip_info(QByteArray data)
{



	QJsonParseError jsonError;
	QJsonDocument doucment = QJsonDocument::fromJson(data, &jsonError);  // 转化为 JSON 文档

if (!doucment.isNull() && (jsonError.error == QJsonParseError::NoError)) {  // 解析未发生错误
    if (doucment.isObject()) {  // JSON 文档为对象
        QJsonObject object = doucment.object();  // 转化为对象
        if (object.contains("address")) {
            QJsonValue value = object.value("address");
            if (value.isString()) {
                QString strName = value.toString();
                qDebug() << "address : " << strName;
                g_addr = strName;
            }
        }

	}

}



}

