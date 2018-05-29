/*
*	Name:		Log_jing.h
*	Abstract:		My own log class to write my own log
*	Version:		1.0
*	Author:		∑Î¬¿›º
*	Date:			2017-8-2
*
*	Copyright (c) 2017£¨–ÌŒ“»˝«ß≥Ë∞Æ All rights reserved.
*/
#ifndef LOG_JING_H
#define LOG_JING_H

#include <time.h>
#include <string>
#include <fstream>
using namespace std;

// the class for log
class Log_jing {
public:
	Log_jing(const char *filename);
	~Log_jing();
public:
	ofstream& debug(string func);
	ofstream& warn(string func);
	ofstream& error(string func);
	bool isFileError();
	//void Write(const char* str, int num);
	//void Write(const char* str, const char* funcName = NULL, int errnum = 0);
private:
	void getCurrentTime();
private:
	ofstream			fout;			// ofstream of the file
	//queue<string>	qStr;			// queue for string
};

// the system log object
extern Log_jing* sysLogJing;
// init the object
bool InitLog(const char *filename);
// destroy the object
void CloseLog();

// no parameter: system log
#define SysDebug() sysLogJing->debug(__FUNCTION__)
#define SysWarn() sysLogJing->warn(__FUNCTION__)
#define SysError() sysLogJing->error(__FUNCTION__)
// one parameter: specific log
#define JingDebug(logObject) logObject->debug(__FUNCTION__)
#define JingWarn(logObject) logObject->warn(__FUNCTION__)
#define JingError(logObject) logObject->error(__FUNCTION__)

#endif // !LOG_JING_H
