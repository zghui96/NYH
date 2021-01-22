#pragma once
#include "Log.h"
#include "common.h"
#include "UserConfig.h"
#include <list>
#include "Record.h"
class F_Waving:public Record
{
private:
	typedef struct FBucket {
		USHORT count;
		bool flags;
		FBucket() :count(0), flags(FALSE) {}
	}FBucket;

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

	double ratio, R_count;
	FBucket** Filter;							//过滤器
	ULONG F_ROW, F_COL;							//过滤器行，列
	ULONG FT;								//过滤器阈值

public:
	F_Waving(const UserConfig& user);
	virtual ~F_Waving();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();

private:
	void getFlowPosition_filter(const FlowID& fid, ULONG* index);
	void insert_identifier(const FlowID& fid);
	bool updataFilter();
	
};

