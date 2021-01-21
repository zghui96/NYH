#include "pch.h"
#include "Log.h"
#include "config.h"
#include <fstream>
#include<windows.h>

void Log::add(string content)
{
    ofstream ofileAgain;
    ofileAgain.open(logPath, ios::app);
    ofileAgain << content << endl;
    ofileAgain.close();
    ofileAgain.clear();
}

void Log::error(string content)
{
    ofstream ofileAgain;
    ofileAgain.open(logErrorPath, ios::app);
    ofileAgain << content << endl;
    ofileAgain.close();
    ofileAgain.clear();
}

void Log::create(const char* path) {
    ofstream ofileAgain;
    ofileAgain.open(path, ios::out);
    ofileAgain.close();
    ofileAgain.clear();
}

void Log::addTest(string content)
{
    ofstream ofileAgain;
    ofileAgain.open(logPathTest, ios::app);
    ofileAgain << content ;
    ofileAgain.close();
    ofileAgain.clear();
}
void Log::addTest_endl()
{
    ofstream ofileAgain;
    ofileAgain.open(logPathTest, ios::app);
    ofileAgain<< endl;
    ofileAgain.close();
    ofileAgain.clear();
}
void Log::addTest_ws(string content)
{
    ofstream ofileAgain;
    ofileAgain.open(logPathTest_ws, ios::app);
    ofileAgain << content ;
    ofileAgain.close();
    ofileAgain.clear();
}

void Log::add_endl()
{
    ofstream ofileAgain;
    ofileAgain.open(logPathTest_ws, ios::app);
    ofileAgain << endl;
    ofileAgain.close();
    ofileAgain.clear();
}

void Log::add_updataFilter(string content)
{
    ofstream ofileAgain;
    ofileAgain.open(logPath_updataFilter, ios::app);
    ofileAgain << content << endl;
    ofileAgain.close();
    ofileAgain.clear();
}
