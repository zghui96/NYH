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
		ULONG count;	//��Ʊ��
		bool flags;		//��־λ

		Entry() :fid(fid), count(0), flags(true) {}


		//����=���������������
		inline Entry& operator=(const Entry& entry) {
			fid = entry.fid;
			count = entry.count;
			return *this;
		}

		//�жϵ�ǰ���Ƿ�Ϊ��
		inline bool isEmpty() {
			return fid.isEmpty() && count == 0;
		}

		//��յ�ǰ��
		inline bool reset() {
			fid.reset();
			count = 0;
			return true;
		}
	}Entry;

	//ͳ�Ʊ�
	Entry** WS;
	// ���˼�����
	long* ws_count;

	//�У�Ͱ�������У��ۣ���
	ULONG ROW, COL;

	double ratio, R_count;
	FBucket** Filter;							//������
	ULONG F_ROW, F_COL;							//�������У���
	ULONG FT;								//��������ֵ

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

