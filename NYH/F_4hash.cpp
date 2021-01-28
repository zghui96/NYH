#include "pch.h"
#include "F_4hash.h"
#include "HashFunctions.h"
#include "config.h"


F_4hash::F_4hash(const UserConfig& user)
{
	F_ROW = user.ning_filter_row;
	F_COL = user.ning_filter_col;
	ratio = (double)(user.ning_ratio * F_ROW * F_COL) / 100;
	R_count = 0;
	T1_ROW = user.ning_T1_row;
	T1_COL = user.ning_T1_col;
	FT = user.ning_ft - 1;
	voteP = user.ning_kickLimit;
	//Filter
	Filter = new FBucket * [F_ROW];
	for (ULONG i = 0; i < F_ROW; i++) {
		Filter[i] = new FBucket[F_COL];
	}
	//recordLayer
	T1 = new Entry * [T1_ROW];
	pVote = new ULONG[T1_ROW];
	for (ULONG i = 0; i < T1_ROW; i++) {
		T1[i] = new Entry[T1_COL];
		pVote[i] = 0;
	}

	//test
	Log::create(logPathTest);
	Log::addTest("选择值：" + to_string(user.m_record) + "\t过滤值：" + to_string(user.ning_ft));
	Log::create(logPath_updataFilter);
}

F_4hash::~F_4hash()
{
	LogTest();
	for (ULONG i = 0; i < F_ROW; i++) {
		delete[] Filter[i];
	}
	delete[] Filter;

	for (ULONG i = 0; i < T1_ROW; i++) {
		delete[] T1[i];
	}
	delete[] T1;
	delete[] pVote;
}

void F_4hash::insert(const FlowID& fid) {
	ULONG index[3], F_min = FT, F_flags = TRUE, F_row = ULONG_MAX, F_col = ULONG_MAX;
	bool pass = TRUE;
	getFlowPosition_filter(fid, index);
	for (ULONG i = 0; i < F_ROW; i++) {
		if (F_min > Filter[i][index[i]].count) {
			F_min = Filter[i][index[i]].count;
			F_row = i;
			F_col = index[i];
		}
		if (Filter[i][index[i]].flags == FALSE) {
			F_flags = FALSE;
		}
	}
	if (F_min < FT && F_flags == FALSE)	pass = FALSE;
	if (F_min < FT) {
		if (Filter[0][index[0]].count == Filter[1][index[1]].count && Filter[1][index[1]].count == Filter[2][index[2]].count) {
			++Filter[0][index[0]].count;
			++Filter[1][index[1]].count;
			++Filter[2][index[2]].count;
			if (Filter[0][index[0]].count == FT) R_count += 3;
		}
		else if (Filter[0][index[0]].count == Filter[1][index[1]].count && Filter[1][index[1]].count == F_min) {
			++Filter[0][index[0]].count;
			++Filter[1][index[1]].count;
			if (Filter[0][index[0]].count == FT) R_count += 2;
		}
		else if (Filter[1][index[1]].count == Filter[2][index[2]].count && Filter[2][index[2]].count == F_min) {
			++Filter[1][index[1]].count;
			++Filter[2][index[2]].count;
			if (Filter[1][index[1]].count == FT) R_count += 2;
		}
		else if (Filter[0][index[0]].count == Filter[2][index[2]].count && Filter[2][index[2]].count == F_min) {
			++Filter[0][index[0]].count;
			++Filter[2][index[2]].count;
			if (Filter[1][index[1]].count == FT) R_count += 2;
		}
		else  if (F_row != ULONG_MAX) {
			++Filter[F_row][F_col].count;
		}
	}
	if (R_count >= ratio) updataFilter();
	if (pass)  insert_identifier(fid);
}

bool F_4hash::updataFilter() {
	/*Log::add_updataFilter("更新前："+ to_string(ratio) + "\t" + to_string(R_count));
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}*/
	//LogTest();
	++updata;
	for (ULONG i = 0; i < F_ROW; i++) {
		for (ULONG j = 0; j < F_COL; j++) {
			if (Filter[i][j].count >= FT)
				Filter[i][j].flags = TRUE;
			else Filter[i][j].flags = FALSE;
			Filter[i][j].count = 0;
		}
	}

	/*Log::add_updataFilter("更新后："+ to_string(ratio) + "\t" + to_string(R_count));
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}*/

	R_count = 0;
	return FALSE;
}

void F_4hash::getFlowPosition_filter(const FlowID& fid, ULONG* index)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	index[0] = RS(buf, FID_LEN) % F_COL;
	index[1] = SBox(buf, FID_LEN) % F_COL;
	index[2] = FNV1(buf, FID_LEN) % F_COL;
}
void F_4hash::getFlowPosition_identifier(const FlowID& fid, ULONG* index) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	uint32_t temp = 0, hash_value = 0;
	hash_value = BOB(buf, FID_LEN);

	temp = hash_value;
	temp = temp >> 24;
	index[0] = temp % T1_ROW;

	temp = hash_value << 8;
	temp = temp >> 24;
	index[1] = (temp % T1_ROW);

	temp = hash_value << 16;
	temp = temp >> 24;
	index[2] = (temp % T1_ROW);

	temp = hash_value << 24;
	temp = temp >> 24;
	index[3] = (temp % T1_ROW);

	//Log::addTest(  to_string(index[0])+"\t"+ to_string(index[1]) + "\t" + to_string(index[2]) + "\t" + to_string(index[3]));
}
bool F_4hash::insert_identifier(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG sign = OAAT(buf, FID_LEN);//指纹
	Entry e(sign);//新建一个条目
	ULONG index[4];
	getFlowPosition_identifier(fid,index);
	
	//检查是否已记录，如没有记录且有空位，则加入
	for (ULONG i = 0; i < 4; i++) {
		for (ULONG j=0; j < T1_COL; j++) {
			if (T1[index[i]][j].sign == e.sign) {		
				++T1[index[i]][j].count;
				return TRUE;
			}
			if (T1[index[i]][j].IsEmpty()) {	
				T1[index[i]][j] = e;
				return TRUE;
			}
		}
	}
	
	ULONG index_row, index_col, min = ULONG_MAX;
	for (ULONG i = 0; i < 4; i++) {
		++pVote[index[i]];
		for (ULONG j=0; j < T1_COL; j++) {
			double fi = pVote[index[i]] / (double)T1[index[i]][j].count;
			if (fi >= voteP) {
				if (min > T1[index[i]][j].count) {
					min = T1[index[i]][j].count;
					index_row = index[i];
					index_col = j;
				}
			}
		}
	}
	
	if (min != ULONG_MAX) {
		T1[index_row] [index_col] = e;
		pVote[index_row] = 0;
	}
	
	return FALSE;
}



ULONG F_4hash::getFlowNum(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG sign = OAAT(buf, FID_LEN);//指纹
	ULONG index[4];
	getFlowPosition_identifier(fid, index);

	for (ULONG i = 0; i < 4; i++) {
		for (ULONG j=0; j < T1_COL; j++) {
			if (T1[index[i]][j].sign == sign) 	
				return T1[index[i]][j].count;
			
		}
	}
	return 0;
}

void F_4hash::LogTest() {

	Log::addTest("ratio:" + to_string(ratio) + "\t\tR_count:" + to_string(R_count) + "\t投票比值的阈值：" + to_string(voteP));
	Log::addTest_endl();
	for (ULONG i = 0; i < T1_ROW; i++) {
		Log::addTest(to_string(pVote[i]) + "\t");
		for (ULONG j = 0; j < T1_COL; j++) {
			Log::addTest( to_string(T1[i][j].count) + "\t");
		}
		Log::addTest_endl();
	}
	Log::add_updataFilter("更新阈值：" + to_string(ratio) + "\t已满个数：" + to_string(R_count) + "\t更新次数：" + to_string(updata));
	Log::add_updataFilter("LogTest：");
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}
}

