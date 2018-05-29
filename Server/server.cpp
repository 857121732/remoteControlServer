#pragma execution_character_set("utf-8")
#include "server.h"

Server::Server(QWidget *parent)
: QWidget(parent), server(5050)
, layout(new QVBoxLayout())
, btnAccept(new QPushButton("允许连接"))
, table(new QTableWidget(this))
{
	ui.setupUi(this);
	// 初始化一些东西
	InitLog("RemoteServer");
	// 显示连接id
	QString serverId = QString::number(GetCodeByIp(GetLocalIpAddress()), 10);
	label = new QLabel("请在客户端输入以下id：\n"+serverId);
	label->setAlignment(Qt::AlignCenter);
	// 表格显示连接的客户端信息
	table->setRowCount(0); //设置行数
	table->setColumnCount(2); //设置列数
	table->setHorizontalHeaderLabels(QStringList());
	table->setItem(0, 0, new QTableWidgetItem("item1")); //设置表格内容
	QStringList header;  //QString类型的List容器  
	header << "连接id" << "客户端描述" ;//设置内容
	table->setHorizontalHeaderLabels(header);//设置表头内容  
	table->verticalHeader()->setVisible(true);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setStyleSheet("background-color:rgba(0,0,0,0)");
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	// 添加布局
	layout->addWidget(label);
	layout->addWidget(btnAccept);
	layout->addWidget(table);
	layout->setAlignment(Qt::AlignCenter);
	setStyleSheet("font-size:20px;");
	this->setLayout(layout);
	// 连接按钮的信号与槽
	connect(btnAccept, SIGNAL(clicked()), this, SLOT(StartUp()));
}

Server::~Server()
{
	delete label;
	delete btnAccept;
	delete table;
	delete layout;
	// 关闭一些东西
	CloseLog();
}

SOCKET Server::StartUp(){
	// 启动server端socket
	SOCKET sock = server.AcceptNewConn();
	int error = server.getError();
	if (error < 0){
		QMessageBox::information(this, QString::fromUtf8("警告"), QString::fromUtf8(string("连接失败！错误码为" + QString::number(error).toUtf8() + "。").c_str()));
		return sock;
	}
	QMessageBox::information(this, QString::fromUtf8("提示"), QString::fromUtf8(string("客户端" + QString::number(sock).toUtf8() + "成功连接！").c_str()));
	// 接收客户端主机名称
	int size;
	string clientName;
	server.RecvData(&size, sizeof(size), sock);
	clientName.resize(size, 0);
	server.RecvData(&clientName[0], size, sock);
	// 为客户端创建新窗口
	ClientWindow *client = new ClientWindow(&server, sock);
	client->setWindowTitle(clientName.c_str());
	client->show();
	clients.insert(make_pair(sock, table->rowCount()));
	// 绑定关闭信号槽
	connect(client, SIGNAL(CloseClient(SOCKET)), this, SLOT(CloseClient(SOCKET)));
	// 在表格中添加客户端信息
	table->insertRow(table->rowCount());
	QTableWidgetItem *newItem = new QTableWidgetItem();
	newItem->setText(QString::number(sock, 10));
	table->setItem(table->rowCount() - 1, 0, newItem);
	QTableWidgetItem *newItem1 = new QTableWidgetItem();
	newItem1->setText(clientName.c_str());
	table->setItem(table->rowCount() - 1, 1, newItem1);
	
	return sock;
}
void Server::CloseClient(SOCKET sock){
	server.CloseConnection(sock);
	table->removeRow(clients[sock]);
	clients.erase(clients.find(sock));
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