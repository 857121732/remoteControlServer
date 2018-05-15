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
#include <queue>
#include <string>
#include <direct.h>
#include <fstream>
using namespace std;

class Log_jing {
public:
	Log_jing(const char *filename);
	~Log_jing();
public:
	void Write(const char* str, int num);
	void Write(const char* str, const char* funcName = NULL, int errnum = 0);
private:
	char				*l_filename;	// filename
	ofstream			fout;			// ofstream of the file
	//queue<string>	qStr;			// queue for string
};

#endif // !LOG_JING_H
