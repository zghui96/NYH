#pragma once
#include "Log.h"
#include "common.h"
#include "UserConfig.h"
#include <list>
#include "Record.h"
class cuckoo_N :public Record
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
		bool flags;
		Entry() :sign(0), count(0),flags(TRUE) {}
		Entry(const ULONG sign) {
			this->sign = sign;
			this->count = 1;
		}
		inline Entry operator=(const Entry& e) {
			sign = e.sign;
			count = e.count;
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
	ULONG FT;								//��������ֵ
	long *WCount;
public:
	cuckoo_N(const UserConfig& user);
	virtual ~cuckoo_N();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();

private:
	void getFlowPosition_filter(const FlowID& fid, ULONG* index);
	bool insert_identifier(const FlowID& fid);
	bool updataFilter();
	void kickOut(Entry e, ULONG row, ULONG col, ULONG kickNum);
};

