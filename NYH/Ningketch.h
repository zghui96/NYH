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
		long count;	//������
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
		//�жϵ�ǰ���Ƿ�Ϊ��
		inline bool IsEmpty() {
			return sign == 0 && count == 0;
		}
	}Entry;


	double ratio, R_count;
	FBucket** Filter;							//������
	ULONG F_ROW, F_COL;							//�������У���
	Entry** T1;								//cuckoo��
	ULONG T1_ROW, T1_COL;
	//long * T1_fVote, *T2_fVote;
	ULONG FT, kickLimit;								//��������ֵ
	double voteThreshold;							//����

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

