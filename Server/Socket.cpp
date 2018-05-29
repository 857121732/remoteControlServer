#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include "Socket.h"

// Constructor and destructor
JingSocket::JingSocket(int port)
: s_listenSock(0), s_log(new Log_jing("SocketServer")), flag(1), error(0)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		JingError(s_log) << "WSAStartup failed : " << WSAGetLastError() << endl;
		error = -1;
		return;
	}

	s_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_listenSock == INVALID_SOCKET) {
		JingError(s_log) << "socket : " << WSAGetLastError() << endl;
		error = -2;
		return;
	}
	SOCKADDR_IN s_serverAddr;
	s_serverAddr.sin_family = AF_INET;
	s_serverAddr.sin_port = htons(port);
	s_serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(s_listenSock, (SOCKADDR*)&s_serverAddr, sizeof(s_serverAddr)) != 0) {
		JingError(s_log) << "bind : " << WSAGetLastError() << endl;
		error = -3;
		return;
	}
	JingDebug(s_log) << "Socket bind!" << endl;
	if (listen(s_listenSock, 5) != 0) {
		JingError(s_log) << "listen : " << WSAGetLastError() << endl;
		error = -4;
		return;
	}
	JingDebug(s_log) << "Make listen socket: socket=" << s_listenSock << endl;
}
JingSocket::JingSocket()
: s_listenSock(0), s_log(new Log_jing("SocketClient")), flag(0), error(0)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		JingError(s_log) << "WSAStartup failed : " << WSAGetLastError() << endl;
		error = -1;
		return;
	}
}
JingSocket::~JingSocket(){
	map<SOCKET, string>::iterator it_sock = s_sock.begin();
	while (it_sock != s_sock.end()) {
		JingDebug(s_log) << "Close socket: sock=" << it_sock->first << " info: " << it_sock->second << endl;
		if (closesocket(it_sock->first) != 0){
			JingError(s_log) << "Close socket failed : " << WSAGetLastError() << endl;
			error = -5;
		}
		++it_sock;
	}
	if (WSACleanup() != 0){
		JingError(s_log) << "WSACleanup failed : " << WSAGetLastError() << endl;
		error = -6;
	}
}

// Accept of server and connect of client
SOCKET JingSocket::AcceptNewConn(){
	SOCKADDR_IN s_clientAddr;
	int	 s_clientAddrLen = sizeof(s_clientAddr);
	SOCKET sock = accept(s_listenSock, (SOCKADDR*)&s_clientAddr, &s_clientAddrLen);
	if (sock == INVALID_SOCKET) {
		JingError(s_log) << "Accept failed : " << WSAGetLastError() << endl;
		error = -7;
		return sock;
	}
	s_sock.insert(make_pair(sock, ""));
	JingDebug(s_log) << "Connect success: sock=" << sock << endl;
	return sock;
}
SOCKET JingSocket::ConnectToServer(int port, const char *str){
	JingDebug(s_log) << "connect : ip=" << str << "&port=" << port << endl;
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		JingError(s_log) << "socket : " << WSAGetLastError() << endl;
		error = -2;
		return sock;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(str);

	if (::connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		JingError(s_log) << "connect : " << WSAGetLastError() << endl;
		error = -8;
		return sock;
	}

	s_sock.insert(make_pair(sock, "server"));
	JingDebug(s_log) << "Connect success: sock=" << sock << endl;
	return sock;
}

// Close connection for server and client
int JingSocket::CloseConnection(SOCKET sock){
	map<SOCKET, string>::iterator it_sock = s_sock.find(sock);
	if (it_sock == s_sock.end()){
		JingError(s_log) << "Socket is not exist : sock=" << sock << endl;
		error = -9;
		return -9;
	}
	if (closesocket(sock) != 0){
		JingError(s_log) << "Close socket failed : " << WSAGetLastError() << endl;
		error = -5;
	}
	else{
		JingDebug(s_log) << "Close socket: sock=" << sock << endl;
		s_sock.erase(it_sock);
	}
	return 0;
}

// Send and receive data or file
int JingSocket::SendData(void* data, size_t size, SOCKET sock) {
	if (s_sock.find(sock) != s_sock.end()){
		return send(sock, (char*)data, size, 0);
	}
	JingError(s_log) << "Socket is not exist : sock=" << sock << endl;
	return -9;
}
int JingSocket::SendFile(const char* filename, SOCKET sock) {
	if (s_sock.find(sock) == s_sock.end()){
		JingError(s_log) << "Socket is not exist : sock=" << sock << endl;
		return -9;
	}
	char sendBuf[1024] = { 0 };

	std::ifstream fin(filename, ios::binary);
	if (!fin.is_open()) {
		JingError(s_log) << "Open file failed: filename=" << filename << endl;
		return -10;
	}
	// Get size of file
	fin.seekg(0, std::ios::end);
	size_t size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	// Send size
	if (send(sock, (char*)&size, sizeof(size), 0) != sizeof(size)){
		JingError(s_log) << "Send data error!" << endl;
		return -11;
	}
	// Send data
	int ret = 0, count = 0;
	ret = fin.read(sendBuf, 1024).gcount();
	while (!fin.eof() && ret > 0) {
		ret = send(sock, sendBuf, ret, 0);
		count += ret;
		ret = fin.read(sendBuf, 1024).gcount();
	}
	if (ret > 0) {
		ret = send(sock, sendBuf, ret, 0);
		count += ret;
	}
	fin.close();
	if (ret > 0 && size != count)
		JingError(s_log) << "Send data error!" << endl;
	return count;
}
int JingSocket::RecvData(void* data, size_t size, SOCKET sock) {
	if (s_sock.find(sock) != s_sock.end()){
		int ret, recvCount = 0;
		while (recvCount < size){
			ret = recv(sock, (char*)data, size-recvCount, 0);
			if (ret <= 0) return ret;
			recvCount += ret;
		}
		return recvCount;
	}
	JingError(s_log) << "Socket is not exist : sock=" << sock << endl;
	return -9;
}
int JingSocket::RecvFile(const char* filename, SOCKET sock) {
	if (s_sock.find(sock) == s_sock.end()){
		JingError(s_log) << "Socket is not exist : sock=" << sock << endl;
		return -9;
	}
	char recvbuff[1024];
	std::ofstream fout(filename, ios::binary);
	if (!fout.is_open()) {
		JingError(s_log) << "Open file failed: filename=" << filename << endl;
		return -10;
	}
	int count = 0, tmp = 0;
	int ret = recv(sock, (char*)&count, sizeof(count), 0);
	while (count > tmp + 1024) {
		ret = recv(sock, recvbuff, 1024, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			JingError(s_log) << "Recv data error!" << endl;
			error = -11;
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	while (count > tmp) {
		ret = recv(sock, recvbuff, count - tmp, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			JingError(s_log) << "Recv data error!" << endl;
			error = -11;
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	fout.close();
	if (count != tmp)
		JingError(s_log) << "Recv data error!" << endl;
	return tmp;
}

int JingSocket::getError(){
	return error;
}