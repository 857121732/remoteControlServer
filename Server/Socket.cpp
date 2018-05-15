#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include "Socket.h"

// ------------------------------------------------
// ------------Begin : class SocketServer----------------
SocketServer::SocketServer(int port)
	: s_listenSock(0), s_log("server.log")
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		s_log.Write("WSAStartup failed!", __FUNCTION__, WSAGetLastError());
		exit(-1);
	}

	s_listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s_listenSock == INVALID_SOCKET) {
		s_log.Write("socket", __FUNCTION__, WSAGetLastError());
		exit(-2);
	}
	SOCKADDR_IN s_serverAddr;
	s_serverAddr.sin_family = AF_INET;
	s_serverAddr.sin_port = htons(port);
	s_serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	if (bind(s_listenSock, (SOCKADDR*)&s_serverAddr, sizeof(s_serverAddr)) != 0) {
		s_log.Write("bind", __FUNCTION__, WSAGetLastError());
		exit(-3);
	}
	s_log.Write("Socket bind!", __FUNCTION__);
	if (listen(s_listenSock, 5) != 0) {
		s_log.Write("listen", __FUNCTION__, WSAGetLastError());
		exit(-4);

	}
	s_log.Write("Make listen socket!", s_listenSock);
}
SocketServer::~SocketServer() {
	for (size_t i = 0; i < s_sock.size(); ++i) {
		s_log.Write("Close socket!", s_sock[i]);
		if (closesocket(s_sock[i]) != 0)
			s_log.Write("closesocket", __FUNCTION__, WSAGetLastError());
	}
	if (WSACleanup() != 0)
		s_log.Write("WSACleanup", __FUNCTION__, WSAGetLastError());
}

int SocketServer::AcceptNewConn() {
	SOCKADDR_IN s_clientAddr;
	int	 s_clientAddrLen = sizeof(s_clientAddr);
	SOCKET sock = accept(s_listenSock, (SOCKADDR*)&s_clientAddr, &s_clientAddrLen);
	if (sock == INVALID_SOCKET) {
		s_log.Write("Accept failed!", __FUNCTION__, WSAGetLastError());
		return sock;
	}
	s_sock.push_back(sock);
	s_log.Write("Connect success!", sock);
	return s_sock.size() - 1;
}
void SocketServer::CloseClient(int idx) {
	if (idx >= 0 && idx < s_sock.size()) {
		int tmp = s_sock[idx];
		if (closesocket(s_sock[idx]) != 0)
			s_log.Write("Closesocket failed!", __FUNCTION__, WSAGetLastError());
		else {
			s_log.Write("Close socket!", tmp);
			for (int i = idx; i < s_sock.size() - 1; ++i)
				s_sock[i] = s_sock[i + 1];
			s_sock.pop_back();
		}
	}
	else
		s_log.Write("Error : The index of socket to close is invalid!", __FUNCTION__);
}

int SocketServer::SendData(void* data, size_t size, int idx) {
	if (idx >= 0 && idx < s_sock.size())
		return send(s_sock[idx], (char*)data, size, 0);
	s_log.Write("Error : The index of socket to send data is invalid!", __FUNCTION__);
	return -1;
}
int SocketServer::SendFile(const char* filename, int readMode, int idx) {
	if (idx < 0 || idx >= s_sock.size()) {
		s_log.Write("Error : The index of socket to send file is invalid!", __FUNCTION__);
		return -1;
	}
	char sendBuf[1024] = { 0 };
	/*HANDLE handle = CreateFile(filename, FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	int size = 0;
	if (handle != INVALID_HANDLE_VALUE) {
	size = GetFileSize(handle, NULL);
	send(s_sock[idx], (char*)&size, sizeof(size), 0);
	CloseHandle(handle);
	}*/
	std::ifstream fin(filename, readMode);
	if (!fin.is_open()) {
		s_log.Write("Error: Open file failed!", __FUNCTION__);
		return -1;
	}
	// Get size of file
	fin.seekg(0, std::ios::end);
	size_t size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	// Send size
	send(s_sock[idx], (char*)&size, sizeof(size), 0);

	// Send data
	int ret = 0, count = 0;
	ret = fin.read(sendBuf, 1024).gcount();
	while (!fin.eof() && ret > 0) {
		ret = send(s_sock[idx], sendBuf, ret, 0);
		count += ret;
		ret = fin.read(sendBuf, 1024).gcount();
	}
	if (ret > 0) {
		ret = send(s_sock[idx], sendBuf, ret, 0);
		count += ret;
	}
	fin.close();
	if (ret > 0 && size != count)
		s_log.Write("Error: File send failed! (size of file != send count)", __FUNCTION__);
	return count;
}

int SocketServer::RecvData(void* data, size_t size, int idx) {
	if (idx >= 0 && idx < s_sock.size())
		return recv(s_sock[s_sock.size() - 1], (char*)data, size, 0);
	s_log.Write("Error : The index of socket to recv data is invalid!", __FUNCTION__);
	return -1;
}
int SocketServer::RecvFile(const char* filename, int readMode, int idx) {
	if (idx < 0 || idx >= s_sock.size()) {
		s_log.Write("Error : The index of socket to recv file is invalid!", __FUNCTION__);
		return -1;
	}
	char recvbuff[1024];
	std::ofstream fout(filename, readMode);
	if (!fout.is_open()) {
		s_log.Write("Error : open file failed!", __FUNCTION__);
		return -1;
	}
	int count = 0, tmp = 0;
	int ret = recv(s_sock[idx], (char*)&count, sizeof(count), 0);
	while (count > tmp + 1024) {
		ret = recv(s_sock[idx], recvbuff, 1024, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			s_log.Write("Recv failed!", __FUNCTION__, GetLastError());
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	while (count > tmp) {
		ret = recv(s_sock[idx], recvbuff, count - tmp, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			s_log.Write("Recv failed!", __FUNCTION__, GetLastError());
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	fout.close();
	if (count != tmp)
		s_log.Write("Error : recieve data size fault!", __FUNCTION__);
	return tmp;
}

// -------------End : class SocketServer---------------
// -----------------------------------------------


// ------------------------------------------------
// ------------Begin : class SocketClient----------------

SocketClient::SocketClient()
	:s_log("client.log")
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		s_log.Write("Error: WSAStartup failed!", __FUNCTION__, WSAGetLastError());
		exit(-1);
	}
}
SocketClient::~SocketClient() {
	for (size_t i = 0; i < s_sock.size(); ++i) {
		s_log.Write("Close socket!", s_sock[i]);
		if (closesocket(s_sock[i]) != 0)
			s_log.Write("closesocket", __FUNCTION__, WSAGetLastError());
	}
	if (WSACleanup() != 0)
		s_log.Write("WSACleanup", __FUNCTION__, WSAGetLastError());
}

int SocketClient::ConnectToServer(int port, const char* str) {
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		s_log.Write("socket", __FUNCTION__, WSAGetLastError());
		return -1;
	}

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.S_un.S_addr = inet_addr(str);

	if (::connect(sock, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR) {
		s_log.Write("connect", __FUNCTION__, WSAGetLastError());
		return -2;
	}

	s_sock.push_back(sock);
	s_log.Write("Connect success!", sock);
	return s_sock.size() - 1;
}
void SocketClient::CloseConnect(int idx) {
	if (idx >= 0 && idx < s_sock.size()) {
		int tmp = s_sock[idx];
		if (closesocket(s_sock[idx]) != 0)
			s_log.Write("closesocket", __FUNCTION__, WSAGetLastError());
		else {
			s_log.Write("Close client socket!", tmp);
			for (int i = idx; i < s_sock.size() - 1; ++i)
				s_sock[i] = s_sock[i + 1];
			s_sock.pop_back();
		}
	}
	else
		s_log.Write("Error : The index of socket to close is invalid!", __FUNCTION__);
}

int SocketClient::SendData(void* data, size_t size, int idx) {
	if (idx >= 0 && idx < s_sock.size())
		return send(s_sock[idx], (char*)data, size, 0);
	s_log.Write("Error : The index of socket to send data is invalid!", __FUNCTION__);
	return -1;
}
int SocketClient::SendFile(const char* filename, int readMode, int idx) {
	if (idx < 0 || idx >= s_sock.size()) {
		s_log.Write("Error : The index of socket to send file is invalid!", __FUNCTION__);
		return -1;
	}
	char sendBuf[1024] = { 0 };
	/*HANDLE handle = CreateFile(filename, FILE_READ_EA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	int size = 0;
	if (handle != INVALID_HANDLE_VALUE) {
	size = GetFileSize(handle, NULL);
	send(s_sock[idx], (char*)&size, sizeof(size), 0);
	CloseHandle(handle);
	}*/
	std::ifstream fin(filename, readMode);
	if (!fin.is_open()) {
		s_log.Write("Error: Open file failed!", __FUNCTION__);
		return -1;
	}
	// Get size of file
	fin.seekg(0, std::ios::end);
	size_t size = fin.tellg();
	fin.seekg(0, std::ios::beg);
	// Send size
	send(s_sock[idx], (char*)&size, sizeof(size), 0);

	// Send data
	int ret = 0, count = 0;
	ret = fin.read(sendBuf, 1024).gcount();
	while (!fin.eof() && ret > 0) {
		ret = send(s_sock[idx], sendBuf, ret, 0);
		count += ret;
		ret = fin.read(sendBuf, 1024).gcount();
	}
	if (ret > 0) {
		ret = send(s_sock[idx], sendBuf, ret, 0);
		count += ret;
	}
	fin.close();
	if (ret > 0 && size != count)
		s_log.Write("Error: File send failed! (size of file != send count)", __FUNCTION__);
	return count;
}

int SocketClient::RecvData(void* data, size_t size, int idx) {
	if (idx >= 0 && idx < s_sock.size())
		return recv(s_sock[s_sock.size() - 1], (char*)data, size, 0);
	s_log.Write("Error : The index of socket to recv data is invalid!", __FUNCTION__);
	return -1;
}
int SocketClient::RecvFile(const char* filename, int readMode, int idx) {
	if (idx < 0 || idx >= s_sock.size()) {
		s_log.Write("Error : The index of socket to recv file is invalid!", __FUNCTION__);
		return -1;
	}
	char recvbuff[1024];
	std::ofstream fout(filename, readMode);
	if (!fout.is_open()) {
		s_log.Write("Error : open file failed!", __FUNCTION__);
		return -1;
	}
	int count = 0, tmp = 0;
	int ret = recv(s_sock[idx], (char*)&count, sizeof(count), 0);
	while (count > tmp + 1024) {
		ret = recv(s_sock[idx], recvbuff, 1024, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			s_log.Write("Recv failed!", __FUNCTION__, GetLastError());
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	while (count > tmp) {
		ret = recv(s_sock[idx], recvbuff, count - tmp, 0);
		if (ret == 0)break;
		else if (ret < 0) {
			s_log.Write("Recv failed!", __FUNCTION__, GetLastError());
			break;
		}
		fout.write(recvbuff, ret);
		tmp += ret;
	}
	fout.close();
	if (count != tmp)
		s_log.Write("Error : recieve data size fault!", __FUNCTION__);
	return tmp;
}
// -------------End : class SocketClient---------------
// -----------------------------------------------
