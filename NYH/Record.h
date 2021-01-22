#pragma once
#include "common.h"
class Record
{
private:
	
public:
	virtual ~Record() {};
	virtual void LogTest()=0;
	virtual void insert(const FlowID& fid) = 0;
	virtual ULONG getFlowNum(const FlowID& fid) = 0;
};