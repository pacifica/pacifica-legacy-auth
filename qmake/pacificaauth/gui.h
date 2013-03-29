#include <QtNetwork>
#include "mainwindow.h"

class PacificaGuiAuth: public QObject
{
	Q_OBJECT
	private:
		MainWindow *mainwindow;
	public:
		PacificaGuiAuth();
	public slots:
		void process(QHash<QString, QString> *);
	signals:
		void ready(QHash<QString, QString> *services);
};
