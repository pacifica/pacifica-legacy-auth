#include <QtGui>
#include <QtNetwork>

class PacificaServices: public QObject
{
	Q_OBJECT
	public:
		PacificaServices();
	protected:
		QHash<QString, QString> *_services;
		QNetworkAccessManager manager;
		QUrl current_url;
		int redirect_count;
	protected slots:
		void downloadFinished(QNetworkReply *reply);
		void sslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
	signals:
		void ready(QHash<QString, QString> *services);
};
