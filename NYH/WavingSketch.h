#pragma once
#include "Log.h"
#include "common.h"
#include <list>
#include "UserConfig.h"

class WavingSketch
{
private:
	typedef struct Entry {
		FlowID fid;		//fid
		ULONG count;	//正票数
		bool flags;		//标志位

		Entry() :fid(fid), count(0), flags(true) {}


		//重载=运算符，拷贝函数
		inline Entry& operator=(const Entry& entry) {
			fid = entry.fid;
			count = entry.count;
			return *this;
		}

		//判断当前项是否为空
		inline bool isEmpty() {
			return fid.isEmpty() && count == 0;
		}

		//清空当前项
		inline bool reset() {
			fid.reset();
			count = 0;
			return true;
		}
	}Entry;

	//统计表
	Entry** WS;
	// 波浪计数器
	long* ws_count;

	//行（桶）数，列（槽）数
	ULONG ROW, COL;

public:
	WavingSketch(const UserConfig& user);
	virtual ~WavingSketch();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();


};

