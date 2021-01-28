#pragma once

#include "Log.h"
#include "common.h"
#include "config.h"
#include "UserConfig.h" 
#include "ningketch.h"
#include "PacketReader.h"
#include "HSNPacketReader.h"
#include "RealCounter.h"
#include "math.h"
#include "WavingSketch.h"
#include "F_Elastic.h"
#include "F_Waving.h"
#include "cuckoo_N.h"
#include "F_4hash.h"

class Statistics
{
private:
	ULONG readNumLimit;	//���ƶ�ȡ���ݰ�����
	Record* ning_identifier;
	WavingSketch* ws_identifier;
	PacketReader* reader;
	RealCounter* realCounter;
	UserConfig* user;
	UserConfig info;

	//test


public:
	/** FMIESketchĬ�Ϲ�����
	* @param1 �û�������
	*/
	Statistics(UserConfig&);
	~Statistics();

	/** ���һ�����ݰ���Sketch
	* @param ���ݰ���Ϣ
	* @return ͳ����Ϸ���true
	*/
	bool add(const Packet&);
	bool arp_add(const Packet&);
	/**
	* ��ʼͳ��
	*/
	void run();
private:

	void init(const UserConfig*);

	/** �������ؼ���fid����Ԫ��
	* @param1 Э������
	* @param2 EndID��Դip��Դ�˿�
	* @param3 EndID��Ŀ��ip��Ŀ�Ķ˿�
	* @return ����fid
	*/
	FlowID calcFlowID(const UCHAR proto, const EndID& src, const EndID& dst);
	void standard();
};

