#ifndef SERVER_H
#define SERVER_H

#include <QtWidgets/QWidget>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qtablewidget.h>
#include "Socket.h"
#include "ui_server.h"
/*
string GetIpByCode(int code){
	string ip = GetLocalIpAddress();
	string result;
	int num[3] = { 0 };
	num[0] = code >> 16;
	num[1] = (code - (num[0] << 16)) >> 8;
	num[2] = code - (num[0] << 16) - (num[1] << 8);
	for (int i = 2; i >= 0; --i){
		if (num[i] == 0)
			result.push_back('0');
		while (num[i] > 0){
			result.push_back((num[i] % 10) + '0');
			num[i] /= 10;
		}
		result.push_back('.');
	}
	result.pop_back();
	reverse(result.begin(), result.end());
	result = string(ip, 0, ip.find('.') + 1) + result;
	return result;
}
*/
class Server : public QWidget
{
	Q_OBJECT

public:
	Server(QWidget *parent = 0);
	~Server();

	static string GetLocalIpAddress();
	static int GetCodeByIp(const string& ip);
private:
	Ui::ServerClass ui;
};

#endif // SERVER_H
