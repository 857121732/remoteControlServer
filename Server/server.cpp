#pragma execution_character_set("utf-8")
#include "server.h"

Server::Server(QWidget *parent)
: QWidget(parent), server(5050)
, layout(new QVBoxLayout())
, btnAccept(new QPushButton("��������"))
, table(new QTableWidget(this))
{
	ui.setupUi(this);
	// ��ʼ��һЩ����
	InitLog("RemoteServer");
	// ��ʾ����id
	QString serverId = QString::number(GetCodeByIp(GetLocalIpAddress()), 10);
	label = new QLabel("���ڿͻ�����������id��\n"+serverId);
	label->setAlignment(Qt::AlignCenter);
	// �����ʾ���ӵĿͻ�����Ϣ
	table->setRowCount(0); //��������
	table->setColumnCount(2); //��������
	table->setHorizontalHeaderLabels(QStringList());
	table->setItem(0, 0, new QTableWidgetItem("item1")); //���ñ������
	QStringList header;  //QString���͵�List����  
	header << "����id" << "�ͻ�������" ;//��������
	table->setHorizontalHeaderLabels(header);//���ñ�ͷ����  
	table->verticalHeader()->setVisible(true);
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	table->setStyleSheet("background-color:rgba(0,0,0,0)");
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	// ��Ӳ���
	layout->addWidget(label);
	layout->addWidget(btnAccept);
	layout->addWidget(table);
	layout->setAlignment(Qt::AlignCenter);
	setStyleSheet("font-size:20px;");
	this->setLayout(layout);
	// ���Ӱ�ť���ź����
	connect(btnAccept, SIGNAL(clicked()), this, SLOT(StartUp()));
}

Server::~Server()
{
	delete label;
	delete btnAccept;
	delete table;
	delete layout;
	// �ر�һЩ����
	CloseLog();
}

SOCKET Server::StartUp(){
	// ����server��socket
	SOCKET sock = server.AcceptNewConn();
	int error = server.getError();
	if (error < 0){
		QMessageBox::information(this, QString::fromUtf8("����"), QString::fromUtf8(string("����ʧ�ܣ�������Ϊ" + QString::number(error).toUtf8() + "��").c_str()));
		return sock;
	}
	QMessageBox::information(this, QString::fromUtf8("��ʾ"), QString::fromUtf8(string("�ͻ���" + QString::number(sock).toUtf8() + "�ɹ����ӣ�").c_str()));
	// ���տͻ�����������
	int size;
	string clientName;
	server.RecvData(&size, sizeof(size), sock);
	clientName.resize(size, 0);
	server.RecvData(&clientName[0], size, sock);
	// Ϊ�ͻ��˴����´���
	ClientWindow *client = new ClientWindow(&server, sock);
	client->setWindowTitle(clientName.c_str());
	client->show();
	clients.insert(make_pair(sock, table->rowCount()));
	// �󶨹ر��źŲ�
	connect(client, SIGNAL(CloseClient(SOCKET)), this, SLOT(CloseClient(SOCKET)));
	// �ڱ������ӿͻ�����Ϣ
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
	memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); // �������ȡ��һ��ip  

	string localIP;
	localIP.assign(inet_ntoa(addr));

	WSACleanup();
	return localIP;
}
int Server::GetCodeByIp(const string& ip){
	// ��ȡ���λ��
	int a[4] = { 0 };
	for (int i = 0, j = 0; i < ip.size(); ++i){
		if (ip[i] == '.')
			a[j++] = i;
	}
	a[3] = ip.size();
	// ��ȡ������ʮ������
	int b[3] = { 0 };
	for (int i = 0; i < 3; ++i){
		for (int j = a[i] + 1; j < a[i + 1]; ++j)
			b[i] = b[i] * 10 + (ip[j] - '0');
	}
	return (b[0] << 16) + (b[1] << 8) + b[2];
}