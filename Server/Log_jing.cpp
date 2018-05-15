#define _CRT_SECURE_NO_WARNINGS 1
#include "Log_jing.h"

Log_jing::Log_jing(const char *filename)
	:l_filename(new char[strlen(filename) + 7])
{
	strcpy(l_filename, "./log/");
	strcpy(l_filename + 6, filename);
	_mkdir("./log");
	fout.open(l_filename, ios::out | ios::app);
}
Log_jing::~Log_jing() {
	if (fout.is_open()) {
		fout.close();
	}
	delete[] l_filename;
}
void Log_jing::Write(const char* str, int num) {
	time_t tm = time(0);
	char tmp[32] = { NULL };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	fout << tmp << '\t' << str << '\t' << num << endl;
}
void Log_jing::Write(const char* str, const char* funcName, int errnum) {
	time_t tm = time(0);
	char tmp[32] = { NULL };
	strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&tm));
	fout << tmp << '\t';
	if (errnum != 0) {
		fout << "Error: " << errnum << '\t';
	}
	fout << str;
	if (funcName != NULL) {
		fout << "\tFunc: " << funcName << endl;
	}
}