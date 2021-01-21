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

public:
	WavingSketch(const UserConfig& user);
	virtual ~WavingSketch();

	void insert(const FlowID& fid);
	ULONG getFlowNum(const FlowID& fid);
	void LogTest();


};

