/*
*	Name:		TcpSocket.h
*	Abstract:		My own socket class to make tcp connection more convenient.
*	Version:		1.0
*	Author:		∑Î¬¿›º
*	Date:			2017-8-2
*
*	Copyright (c) 2017£¨–ÌŒ“»˝«ß≥Ë∞Æ All rights reserved.
*/

#ifndef SOCKET_H
#define SOCKET_H

#pragma comment(lib, "Ws2_32.lib")

#include "Log_jing.h"
#include <WS2tcpip.h>
#include <WinSock2.h>

class SocketServer {
public:
	SocketServer(int port);
	~SocketServer();
public:
	int AcceptNewConn();
	void CloseClient(int idx = 0);
	int SendData(void* data, size_t size, int idx = 0);
	int SendFile(const char* filename, int readMode, int idx = 0);
	int RecvData(void* data, size_t size, int idx = 0);
	int RecvFile(const char* filename, int readMode, int idx = 0);
private:
	std::vector<SOCKET>	s_sock;				// sockets for clients
	SOCKET					s_listenSock;			// listening socket
	Log_jing					s_log;					// log struct to take note
private:	// ban call the following two functions
	SocketServer(SocketServer& other);		// copy constructor
	void operator = (SocketServer other) {}	// operator = 
};

class SocketClient {
public:
	SocketClient();
	~SocketClient();
public:
	int ConnectToServer(int port, const char* str);
	void CloseConnect(int idx = 0);

	int SendData(void* data, size_t size, int idx = 0);
	int SendFile(const char* filename, int readMode, int idx = 0);
	int RecvData(void* data, size_t size, int idx = 0);
	int RecvFile(const char* filename, int readMode, int idx = 0);
private:
	std::vector<SOCKET>	s_sock;				// sockets for servers
	Log_jing					s_log;					// log struct to take note
private:	// ban call the following two functions
	SocketClient(SocketClient& other);			// copy constructor
	void operator = (SocketClient other) {}	// operator = 
};
#endif // !SOCKET_H
