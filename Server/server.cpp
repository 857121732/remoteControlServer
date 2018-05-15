#pragma execution_character_set("utf-8")
#include "server.h"

Server::Server(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	QVBoxLayout *layout = new QVBoxLayout();

	QLabel *label1 = new QLabel("请在客户端输入以下id：");

	QString serverId = QString::number(GetCodeByIp(GetLocalIpAddress()), 10);
	QLabel *label2 = new QLabel(serverId);

	QTableWidget* table = new QTableWidget(this); //创建一个表格
	table->setRowCount(3); //设置行数
	table->setColumnCount(2); //设置列数
	table->setHorizontalHeaderLabels(QStringList());
	table->setItem(0, 0, new QTableWidgetItem("item1")); //设置表格内容

	layout->addWidget(label1);
	layout->addWidget(label2);
	layout->addWidget(table);

	setStyleSheet("font-family: ");
	this->setLayout(layout);
}

Server::~Server()
{

}

string Server::GetLocalIpAddress(){
	WORD wVersionRequested = MAKEWORD(2, 2);

	WSADATA wsaData;
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
		return "";

	char local[255] = { 0 };
	gethostname(local, sizeof(local));
	hostent* ph = gethostbyname(local);
	if (ph == NULL)
		return "";

	in_addr addr;
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // 这里仅获取第一个ip  

	string localIP;
	localIP.assign(inet_ntoa(addr));

	WSACleanup();
	return localIP;
}
int Server::GetCodeByIp(const string& ip){
	// 获取点的位置
	int a[4] = { 0 };
	for (int i = 0, j = 0; i < ip.size(); ++i){
		if (ip[i] == '.')
			a[j++] = i;
	}
	a[3] = ip.size();
	// 获取后三个十进制数
	int b[3] = { 0 };
	for (int i = 0; i < 3; ++i){
		for (int j = a[i] + 1; j < a[i + 1]; ++j)
			b[i] = b[i] * 10 + (ip[j] - '0');
	}
	return (b[0] << 16) + (b[1] << 8) + b[2];
}