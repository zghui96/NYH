#pragma once
#include "Log.h"
#include "common.h"
#include "UserConfig.h"
#include <list>

class Ningketch
{
private:
	typedef struct FBucket {
		USHORT count;
		bool flags;
		FBucket() :count(0), flags(FALSE) {}
	}FBucket;

	typedef struct Entry {
		ULONG sign;		//fid
		long count;	//计数器
		long WCount;
		Entry() :sign(0), count(0), WCount(0) {}
		Entry(const ULONG sign) {
			this->sign = sign;
			this->count = 1;
			WCount = 0;
		}
		inline Entry operator=(const Entry& e) {
			sign = e.sign;
			count = e.count;
			WCount = e.WCount;
			return *this;
		}
		//判断当前项是否为空
		inline bool IsEmpty() {
			return sign == 0 && count == 0;
		}
	}Entry;


	double ratio, R_count;
	FBucket** Filter;							//过滤器
	ULONG F_ROW, F_COL;							//过滤器行，列
	Entry** T1;								//cuckoo表
	ULONG T1_ROW, T1_COL;
	//long * T1_fVote, *T2_fVote;
	ULONG FT, kickLimit;								//过滤器阈值
	double voteThreshold;							//参数

public:
	Ningketch(const UserConfig& user);
	virtual ~Ningketch();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();

private:
	void getFlowPosition_filter(const FlowID& fid, ULONG* index);
	bool insert_identifier(const FlowID& fid);
	bool updataFilter();
	void kickOut(Entry e,ULONG row,ULONG col,ULONG kickNum);
};

