#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include <QPointer>
#include <iostream>
#include "mainwindow.h"
#include "ui_authenticationdialog.h"
#include "ui_pacificaauth.h"

void MainWindow::authentication_required(QNetworkReply * reply, QAuthenticator *authenticator)
{
	QDialog dialog;
	Ui::Dialog ui;
	ui.setupUi(&dialog);
	ui.site->setText(tr("%1").arg(authenticator->realm()));
	dialog.adjustSize();
	this->hide();
	if(dialog.exec() == QDialog::Accepted)
	{
		authenticator->setUser(ui.user->text());
		authenticator->setPassword(ui.password->text());
	}
	else
	{
	    exit(-1);
	}
	this->show();
}

void MyNetworkAccessManager::my_finished(QNetworkReply *reply)
{
	if(reply->error())
	{
		std::cout << "Error: " << reply->error() << "\n\n";
	}
}

QNetworkReply *MyNetworkAccessManager::createRequest(Operation op, const QNetworkRequest &req, QIODevice* outgoingData)
{
	QNetworkReply *rep;
//	QSslConfiguration config = req.sslConfiguration();
	QNetworkRequest request(req);
/*
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::TlsV1);
	request.setSslConfiguration(config);
*/
	rep = QNetworkAccessManager::createRequest(op, request, outgoingData);
	connect(this, SIGNAL(finished(QNetworkReply *)), SLOT(my_finished(QNetworkReply *)));
	return rep;
}

void MainWindow::ssl_errors(QNetworkReply * reply, const QList<QSslError> & errors)
{
	QSettings settings;
	if(settings.contains("url/insecure") && settings.value("url/insecure").toBool())
	{
		reply->ignoreSslErrors();
	}
}

MainWindow::MainWindow(const QUrl& url)
{
	testauth = url;
	uimw = new Ui::mainWindow();
	uimw->setupUi(this);
	this->show();
	progress = 0;
	QPointer<QWebPage> _webPage = new QWebPage();
	QPointer<MyNetworkAccessManager> am = new MyNetworkAccessManager();
//	QPointer<QNetworkAccessManager> am = QPointer<QNetworkAccessManager> Imam;
	connect(am, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)), SLOT(authentication_required(QNetworkReply *, QAuthenticator *)));
	connect(am, SIGNAL(sslErrors(QNetworkReply *, const QList<QSslError> &)), SLOT(ssl_errors(QNetworkReply *, const QList<QSslError> &)));
	uimw->webView->setPage(_webPage);
	_webPage -> setNetworkAccessManager(am);
	uimw->webView->load(url);
	connect(uimw->webView, SIGNAL(loadProgress(int)), SLOT(set_progress(int)));
	connect(uimw->webView, SIGNAL(loadFinished(bool)), SLOT(finished_loading(bool)));
	setUnifiedTitleAndToolBarOnMac(true);
//	this->hide();
}

void MainWindow::change_location(QUrl url)
{
    uimw->webView->load(url);
    uimw->webView->setFocus();
}

void MainWindow::adjust_title()
{
	if(progress <= 0 || progress >= 100)
	{
		setWindowTitle(uimw->webView->title());
	}
	else
	{
		setWindowTitle(QString("%1 (%2%)").arg(uimw->webView->title()).arg(progress));
	}
}

void MainWindow::set_progress(int p)
{
    progress = p;
    adjust_title();
}

void MainWindow::finished_loading(bool ok)
{
	if(!ok)
std::cout << "LOAD FINISH NOT OK!\n\n";
	QString output;
	QWebElement body;
	progress = 100;
	adjust_title();
	QWebFrame *frame = uimw->webView->page()->mainFrame();
	QWebElement document = frame->documentElement();
	QWebElement element = document.firstChild();
	while(!element.isNull())
	{
		if(element.tagName() == "BODY")
		{
			output = element.toInnerXml();
			if(output == "ok\n")
			{
				QNetworkCookieJar *cj = uimw->webView->page()->networkAccessManager()->cookieJar();
				QList<QNetworkCookie> cookies = cj->cookiesForUrl(testauth);
				for(QList<QNetworkCookie>::const_iterator i = cookies.begin() ; i != cookies.end() ; i++ )
				{
					std::cout << "Set-Cookie: ";
					QByteArray ba = i->toRawForm();
					std::cout.write(ba.data(), ba.count());
					std::cout << "\r\n";
				}
				exit(0);
			}
		}
		element = element.nextSibling();
	}
	this->show();
}

//FIXME close window should exit 1
