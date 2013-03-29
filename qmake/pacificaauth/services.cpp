#include <QtGui>
#include <QtXml>
#include <QtNetwork>
#include <iostream>
#include "services.h"

PacificaServices::PacificaServices()
{
	const char *services = "https://my.emsl.pnl.gov/myemsl/services/";
	QSettings settings;
	if(settings.contains("url/services"))
	{
		services = strdup(settings.value("url/services").toString().toStdString().c_str());
	}
	current_url = QUrl(services);
	redirect_count = 0;
	QNetworkRequest request(current_url);
	QNetworkReply *reply = manager.get(request);
	connect(&manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(downloadFinished(QNetworkReply *)));
	connect(&manager, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), this, SLOT(sslErrors(QNetworkReply *, const QList<QSslError> &)));
	std::cout << "Started!\n";
}
void PacificaServices::downloadFinished(QNetworkReply *reply)
{
	const char *prefix = "";
	QDomElement root;
	QDomNode services;
	QDomNode n;
	QDomElement e;
	int i;
	int res;
	QUrl url;
	QVariant possible_redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	url = possible_redirect.toUrl();
	if(!url.isEmpty() && current_url != url && redirect_count < 5)
	{
		redirect_count++;
		current_url = url;
std::cout << "Redirecting to " << url.toString().toStdString() << "\n";
		QNetworkRequest request(current_url);
		QNetworkReply *reply = manager.get(request);
		return;

	}
//FIXME handle error.
	_services = new QHash<QString, QString>();
//FIXME rename root element once server is updated.
	QDomDocument doc("myemsl");
	std::cout << doc.setContent(reply) << "\n";
	root = doc.documentElement();
	QDomNodeList list = root.elementsByTagName("prefix");
	for(i = 0; i < list.count(); i++)
	{
		e = list.at(i).toElement();
		prefix = strdup(e.text().toStdString().c_str());
	}
	QSettings settings;
	if(settings.contains("url/prefix"))
	{
		prefix = strdup(settings.value("url/prefix").toString().toStdString().c_str());
	}
	list = root.elementsByTagName("services");
	for(i = 0; i < list.count(); i++)
	{
		services = list.at(i);
	}
	list = services.childNodes();
	for(i = 0; i < list.count(); i++)
	{
		e = list.at(i).toElement();
		if(e.nodeName() == "service")
		{
			if(e.attribute("location", "").toStdString().c_str()[0] == '/')
			{
				_services->insert(e.attribute("name", NULL), prefix + e.attribute("location", ""));
			}
			else
			{
				_services->insert(e.attribute("name", NULL), e.attribute("location", ""));
			}
		}
	}
	ready(_services);
}

void PacificaServices::sslErrors(QNetworkReply *reply, const QList<QSslError> &errors)
{
	QSettings settings;
	if(settings.contains("url/insecure") && settings.value("url/insecure").toBool())
	{
		reply->ignoreSslErrors();
	}
}
