#ifndef LOGGER_H
#define LOGGER_H

#include<sys/stat.h>
#include<iostream>
#include<fstream>

using namespace std;

class Logger{

    string logFile = "log";
    ofstream fout;

public:

    Logger();
    void log(string logString);
};

extern Logger logger;

#endif // LOGGER_H



