#pragma once
#pragma once

#include "common.h"
#include <list>
#include <string>

using namespace std;
typedef struct UserConfig {
	list<string> fileList;					//数据包文件列表
	ULONG PACKET_NUM_LIMIT;
	ULONG realLargeFlowNum;

	//ning-ketch
	ULONG ning_ratio;
	ULONG ning_filter_row;
	ULONG ning_filter_col;
	ULONG ning_T1_row;
	ULONG ning_T1_col;
	ULONG ning_T2_row;
	ULONG ning_T2_col;
	double ning_voteThreshold;
	ULONG ning_ft;
	ULONG ning_kickLimit;
	ULONG ning_judgeNum;
	double ning_AAE, ning_ARE;

	//WS参数
	ULONG WS_row;
	ULONG WS_col;
	ULONG WS_judgeNum;
	double WS_AAE, WS_ARE;
}UserConfig, * pUserConfig;

