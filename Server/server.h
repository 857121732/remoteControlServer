#ifndef SERVER_H
#define SERVER_H

#include <QtWidgets/QWidget>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include <qmessagebox.h>
#include <map>
#include "Socket.h"
#include "clientWindow.h"
#include "ui_server.h"

class Server : public QWidget
{
	Q_OBJECT

public:
	Server(QWidget *parent = 0);
	~Server();

public slots:
	SOCKET StartUp();
	void CloseClient(SOCKET sock);
private:
	static string GetLocalIpAddress();
	static int GetCodeByIp(const string& ip);
private:
	Ui::ServerClass	ui;
	JingSocket		server;
	QVBoxLayout	*layout;
	QTableWidget	*table;
	QPushButton	*btnAccept;
	QLabel			*label;
	map<SOCKET, int> clients;
};

#endif // SERVER_H
