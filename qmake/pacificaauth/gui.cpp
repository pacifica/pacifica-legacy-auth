#include <QtGui>
#include <iostream>
#include "gui.h"

PacificaGuiAuth::PacificaGuiAuth()
{

}

void PacificaGuiAuth::process(QHash<QString, QString> *services)
{
	QUrl url(services->value("testauth"));
	mainwindow = new MainWindow(url);
	std::cout << "Gui Started. " << services->value("testauth").toStdString() << "\n";
	ready(services);
}
