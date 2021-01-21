
#pragma once

typedef unsigned long ULONG;

const static char* logPath_updataFilter = "E:/ning-updataFilter.log";
const static char* logPathTest = "E:/ning-test.log";
const static char* logPathTest_ws = "E:/ning-test_ws.log";
const static char* logPath = "E:/ning-Sketch.log";
const static char* logErrorPath = "E:/ning-Sketch-error.log";
const static char* resultPath = "E:/ning-Sketch-result.txt";
const static unsigned long HASH_LEN = 4096;

const static ULONG KICK_OUT_NUM = 2;	// 过滤器最大踢除次数
const static ULONG LARGE_FLOW_REAL_THRESHOLD = 500;	//500个数据包则被判断为大流，用于真实统计
const static ULONG LARGE_FLOW_LFCOUNTER_THRESHOLD = 500;	// 大流统计器中判定为大流的阈值
const static ULONG LARGE_FLWO_CMSKETCH_THRESHOLD = 500;		// CMSketch判断为大流的阈值
