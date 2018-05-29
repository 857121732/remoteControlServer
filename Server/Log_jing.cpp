#include "Log_jing.h"


char tmpTime[32] = { 0 };
Log_jing* sysLogJing = NULL;

Log_jing::Log_jing(const char *filename)
{
	getCurrentTime();
	string filePath = string(filename) + "_" + tmpTime + ".log";
	fout.open(filePath, ios::out | ios::app);
}
Log_jing::~Log_jing() {
	if (fout.is_open()) {
		fout.close();
	}
}

// Three log types
ofstream& Log_jing::debug(string func){
	getCurrentTime();
	fout << "[" << tmpTime << " " << func << "][debug]";
	return fout;
}
ofstream& Log_jing::warn(string func){
	getCurrentTime();
	fout << "[" << tmpTime << " " << func << "][ warn]";
	return fout;
}
ofstream& Log_jing::error(string func){
	getCurrentTime();
	fout << "[" << tmpTime << " " << func << "][error]";
	return fout;
}
// Whether the file creation was successful
bool Log_jing::isFileError(){
	return !fout;
}
// Get current time to write log
void Log_jing::getCurrentTime(){
	time_t myTime = time(NULL);
	strftime(tmpTime, sizeof(tmpTime), "%Y_%m_%d_%H_%M_%S", localtime(&myTime));
}

bool InitLog(const char *filename){
	sysLogJing = new Log_jing(filename);
	return !sysLogJing->isFileError();
}
void CloseLog(){
	delete sysLogJing;
}