#pragma once
/**
*	��־�࣬���ڱ��������־
*/

#include <iostream>
using namespace std;

class Log
{
public:
	static void add(string content);	// ������־
	static void create(const char* path);	// �����ļ�
	static void error(string content);	// ������־
	static void addTest(string content);	
	static void addTest_endl();
	static void addTest_ws(string content);	
	static void add_endl();
	static void add_updataFilter(string content);
};

