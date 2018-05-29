#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <qthread.h>
#include <qthreadpool.h>
#include "server.h"

class myThread : public QThread{
public:
	myThread(Server *server);
	~myThread();
public:
	void run();
private:
	Server *Tserver;
signals:
	void getSocket(int){}
};


#endif