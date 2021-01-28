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
	ULONG readNumLimit;	//限制读取数据包个数
	Record* ning_identifier;
	WavingSketch* ws_identifier;
	PacketReader* reader;
	RealCounter* realCounter;
	UserConfig* user;
	UserConfig info;

	//test


public:
	/** FMIESketch默认构造器
	* @param1 用户配置项
	*/
	Statistics(UserConfig&);
	~Statistics();

	/** 添加一个数据包到Sketch
	* @param 数据包信息
	* @return 统计完毕返回true
	*/
	bool add(const Packet&);
	bool arp_add(const Packet&);
	/**
	* 开始统计
	*/
	void run();
private:

	void init(const UserConfig*);

	/** 计算流关键字fid，五元组
	* @param1 协议类型
	* @param2 EndID，源ip、源端口
	* @param3 EndID，目的ip、目的端口
	* @return 返回fid
	*/
	FlowID calcFlowID(const UCHAR proto, const EndID& src, const EndID& dst);
	void standard();
};

