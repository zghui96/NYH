#pragma once
#include "Log.h"
#include "common.h"
#include "UserConfig.h"
#include <list>
#include "Record.h"

class F_Elastic:public Record
{
private:
	typedef struct FBucket {
		USHORT count;
		bool flags;
		FBucket() :count(0), flags(FALSE) {}
	}FBucket;

	typedef struct Entry {
		FlowID fid;		//fid
		ULONG pVote;	//计数器
		
		Entry() :pVote(0) {}
		
		//判断当前项是否为空
		inline bool isEmpty() {
			return fid.isEmpty() && pVote == 0;
		}
		//清空当前项
		inline bool reset() {
			fid.reset();
			pVote = 0;
			return true;
		}
	}Entry;


	double ratio, R_count;
	FBucket** Filter;							//过滤器
	ULONG F_ROW, F_COL;							//过滤器行，列
	Entry** entryTable;								//cuckoo表
	ULONG* fVotes;
	ULONG ROW_NUM, COL_NUM;
	//long * T1_fVote, *T2_fVote;
	ULONG FT;								//过滤器阈值
	double voteThreshold;							//参数

public:
	F_Elastic(const UserConfig& user);
	virtual ~F_Elastic();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();

private:
	void getFlowPosition_filter(const FlowID& fid, ULONG* index);
	bool insert_identifier(const FlowID& fid);
	bool updataFilter();
	
	void getFlowPosition(const FlowID& fid, Pair<ULONG, ULONG>&);
	bool checkAndReset(ULONG);

};

