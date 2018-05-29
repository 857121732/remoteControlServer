/*
*	Name:		TcpSocket.h
*	Abstract:		My own socket class to make tcp connection more convenient.
*	Version:		2.0
*	Author:		∑Î¬¿›º
*	Date:			2017-8-2
*
*	Copyright (c) 2017£¨–ÌŒ“»˝«ß≥Ë∞Æ All rights reserved.
*/

#ifndef SOCKET_H
#define SOCKET_H

#pragma comment(lib, "Ws2_32.lib")

#include "Log_jing.h"
#include <map>
#include <WS2tcpip.h>
#include <WinSock2.h>

class JingSocket{

public:
	JingSocket(int port);						// constructor for server
	JingSocket();	// constructor for client
	~JingSocket();
public:
	SOCKET AcceptNewConn();
	SOCKET ConnectToServer(int port, const char* str);
	int CloseConnection(SOCKET sock);
public:
	int SendData(void* data, size_t size, SOCKET sock);
	int SendFile(const char* filename, SOCKET sock);
	int RecvData(void* data, size_t size, SOCKET sock);
	int RecvFile(const char* filename, SOCKET sock);
	int getError();
private:
	std::map<SOCKET, string>	s_sock;				// sockets for clients
	SOCKET					s_listenSock;	// listening socket
	Log_jing					*s_log;		// log struct to take note
	bool						flag;		// flag for server(flag=1)
	int							error;		// the current error code(0 is null)
private:	// ban call the following two functions
	JingSocket(JingSocket& other);		// copy constructor
	void operator = (JingSocket other) {}	// operator = 

};

#endif // !SOCKET_H
