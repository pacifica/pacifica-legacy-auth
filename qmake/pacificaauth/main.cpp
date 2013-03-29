#include <QtGui>
#include <QtNetwork>
#include <iostream>
#include <curl/curl.h>
#include "ui_pacificaauth.h"
#include "mainwindow.h"
#include "services.h"
#include "kerberos.h"
#include "gui.h"
#include <string.h>

int main(int argc, char * argv[])
{
	const char *testauth = "https://my.emsl.pnl.gov/myemsl/testauth/";
	int i;
	int enable_kerberos = true;
	int res;
	QApplication app(argc, argv);
	QUrl url;
	QCoreApplication::setOrganizationName("Pacifica");
	QCoreApplication::setOrganizationDomain("my.emsl.pnl.gov");
	QCoreApplication::setApplicationName("Pacifica Auth");
	QSettings settings;
//Get and interpret services file using qt fetcher and xml parser here. Convert to "Service: name=value\r\n" lines.
//	settings.setValue("url/testauth", "https://myemsl-dev5.emsl.pnl.gov/myemsl/testauth/");
//	settings.sync();
	if(settings.contains("url/testauth"))
	{
		testauth = strdup(settings.value("url/testauth").toString().toStdString().c_str());
	}
	url = QUrl(testauth);

//	std::cout << "URL: |" << testauth << "|\r\n";
	for(i = 0; i < argc; i++) 
	{
		if(!strcmp(argv[i], "--disable-kerberos"))
		{
			enable_kerberos = false;
		}
	}
	if(settings.contains("url/proxy_host"))
	{
		QNetworkProxy proxy;
 		proxy.setType(QNetworkProxy::HttpProxy);
		proxy.setHostName(settings.value("url/proxy_host").toString());
		proxy.setPort(settings.value("url/proxy_port").toInt());
		QNetworkProxy::setApplicationProxy(proxy);
		std::cout << "Setting: proxy_host=" << settings.value("url/proxy_host").toString().toStdString() << "\r\n";
		std::cout << "Setting: proxy_port=" << settings.value("url/proxy_port").toString().toStdString() << "\r\n";
	}
	else
	{
		QNetworkProxyFactory::setUseSystemConfiguration(true);
	}
	if(settings.contains("url/insecure") && settings.value("url/insecure").toBool())
	{
		std::cout << "Setting: insecure\r\n";
	}
	PacificaKerberosAuth ka(!enable_kerberos);
PacificaServices services;
QObject::connect(&services, SIGNAL(ready(QHash<QString, QString> *)), &ka, SLOT(process(QHash<QString, QString> *)));
	PacificaGuiAuth ga;
QObject::connect(&ka, SIGNAL(ready(QHash<QString, QString> *)), &ga, SLOT(process(QHash<QString, QString> *)));
//	MainWindow *mainwindow = new MainWindow(url);
	std::cout << "Goin.\n";
	res = app.exec();
	std::cout << "Exiting " << res << "\n";
	return 1;
}
