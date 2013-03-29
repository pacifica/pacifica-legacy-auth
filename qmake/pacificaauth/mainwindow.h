#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QtNetwork>
#include <QtWebKit>
#include <QPointer>

class Ui_mainWindow;

class MyNetworkAccessManager: public QNetworkAccessManager
{
	Q_OBJECT
	public:
		QNetworkReply *createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData);
	protected slots:
		void my_finished(QNetworkReply *reply);
};

class MainWindow: public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(const QUrl &url);
	void change_location(QUrl url);
private:
	int progress;
	Ui_mainWindow *uimw;
	QUrl testauth;
protected slots:
	void set_progress(int p);
	void adjust_title();
	void authentication_required (QNetworkReply *reply, QAuthenticator *authenticator);
	void ssl_errors(QNetworkReply *reply, const QList<QSslError> &errors);
	void finished_loading(bool done);
};

#endif // MAINWINDOW_H
