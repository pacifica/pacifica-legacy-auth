#include <QtNetwork>

class PacificaKerberosAuth: public QObject
{
	Q_OBJECT
	private:
		bool _skip_kerberos;
	public:
		PacificaKerberosAuth(bool skip_kerberos);
	public slots:
		void process(QHash<QString, QString> *);
	signals:
		void kerberosFailed(QHash<QString, QString> *);
		void ready(QHash<QString, QString> *services);
};
