#include "myThread.h"

myThread::myThread(Server *server)
:Tserver(server){}

myThread::~myThread(){}

void myThread::run(){
	SOCKET sock = Tserver->StartUp();
	emit getSocket((int)sock);
}