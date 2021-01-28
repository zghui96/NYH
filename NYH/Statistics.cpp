#include "pch.h"
#include "Statistics.h"
#include <list>

Statistics::Statistics(UserConfig& info) :readNumLimit(info.PACKET_NUM_LIMIT), info(info)
{
	user = &info;
	srand(unsigned(time(NULL)));
	init(user);
}

Statistics::~Statistics()
{
	if (ws_identifier != NULL) {
		delete ws_identifier;
		ws_identifier = NULL;
	}
	if (ning_identifier != NULL) {
		delete ning_identifier;
		ning_identifier = NULL;
	}
	if (reader != NULL) {
		delete reader;
		reader = NULL;
	}
	if (realCounter != NULL) {
		delete realCounter;
		realCounter = NULL;
	}
}

void Statistics::init(const UserConfig* user)
{
	ASSERT(!user->fileList.empty());

	//ningketch测量
	
	switch (user->m_record) {
	case 0: {
		ning_identifier = new Ningketch(info); break;
	}
	case 1: {
		ning_identifier = new cuckoo_N(info); break;
	}
	case 2: {
		ning_identifier = new F_Elastic(info); break;
	}
	case 3: {
		ning_identifier = new F_4hash(info); break;
	}
	case 4: {
		ning_identifier = new F_Waving(info); break;
	}

	}
	

	//ws测量
	ws_identifier = new WavingSketch(info);

	reader = new HSNPacketReader();
	list<string> strList = user->fileList;
	list<string>::iterator iter;
	for (iter = strList.begin(); iter != strList.end(); iter++)
	{
		reader->addFile(*iter);
	}
	realCounter = new RealCounter(LARGE_FLOW_REAL_THRESHOLD);
}

FlowID Statistics::calcFlowID(const UCHAR proto, const EndID& src, const EndID& dst)
{
	FlowID fid;
	fid.proto = proto;
	fid.src = src;
	fid.dst = dst;
	return fid;
}
// 统计流
bool Statistics::add(const Packet& pkt)
{
	ASSERT(pkt.proto == PROTO_TCP || pkt.proto == PROTO_UDP);
	// 计算流id
	const FlowID fid = calcFlowID(pkt.proto, pkt.src, pkt.dst);

	realCounter->add(fid);	//加到真实的统计器中
	ning_identifier->insert(fid);	//加到ningketch的统计器中
	ws_identifier->insert(fid);	//加到ws的统计器中

	return true;
}




void Statistics::standard() {

	list<FlowID*> flowListRealCounter;
	list<ULONG> flowNumListRealCounter;
	realCounter->getLargeFlowList(flowListRealCounter, flowNumListRealCounter);
	list<ULONG>::iterator numIter;
	list<FlowID*>::iterator iter;
	ULONG i = 0, ning_fNum = 0, ning_judgeNum = 0, ws_fNum = 0, ws_judgeNum = 0;
	double   ning_sum_AAE = 0, ning_sum_ARE = 0, ws_sum_AAE = 0, ws_sum_ARE = 0;

	ULONG realLargeFlowNum = flowListRealCounter.size();	//真实样本中的大流数量
	Log::create(logPath);
	Log::add("流编号\t真实数量\tning-ketch\tWavingSketch");
	for (iter = flowListRealCounter.begin(), numIter = flowNumListRealCounter.begin(); iter != flowListRealCounter.end() && numIter != flowNumListRealCounter.end(); iter++, numIter++)
	{


		ning_fNum = 0;
		ning_fNum = ning_identifier->getFlowNum(**iter);
		if (ning_fNum > 0) ning_fNum += user->ning_ft;
		if (ning_fNum > LARGE_FLOW_REAL_THRESHOLD) ++ning_judgeNum;
		ning_sum_AAE += abs((long)(*numIter - ning_fNum));
		ning_sum_ARE += (abs((long)(*numIter - ning_fNum)) / (double)*numIter);

		ws_fNum = 0;
		ws_fNum = ws_identifier->getFlowNum(**iter);
		if (ws_fNum > LARGE_FLOW_REAL_THRESHOLD) ++ws_judgeNum;
		ws_sum_AAE += abs((long)(*numIter - ws_fNum));
		ws_sum_ARE += (abs((long)(*numIter - ws_fNum)) / (double)*numIter);

		Log::add("f" + to_string(i++) + "\t" + to_string(*numIter)+ "\t\t" + to_string(ning_fNum) + "\t" + to_string(ws_fNum));
	}
	Log::add("大流数量:\t" + to_string(realLargeFlowNum)  + "\t" + to_string(ning_judgeNum) + "\t" + to_string(ws_judgeNum));
	//界面参数获取新值
	user->realLargeFlowNum = realLargeFlowNum;

	//界面ningketch参数获取新值
	user->ning_judgeNum = ning_judgeNum;
	user->ning_AAE = ning_sum_AAE / realLargeFlowNum;
	user->ning_ARE = ning_sum_ARE / realLargeFlowNum;

	//界面ws参数获取新值
	user->WS_judgeNum = ws_judgeNum;
	user->WS_AAE = ws_sum_AAE / realLargeFlowNum;
	user->WS_ARE = ws_sum_ARE / realLargeFlowNum;

	//ning_identifier->LogTest();
	//ws_identifier->LogTest();
}



void Statistics::run()
{
	Packet pkt;
	ULONG countNum = 0;

	//1、统计流

	while (countNum < readNumLimit && reader->readPacket(pkt)) {
		bool flag = add(pkt);
		if (flag)countNum++;
	}
	//2、计算标准
	standard();
}