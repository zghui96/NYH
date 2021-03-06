#include "pch.h"
#include "cuckoo_N.h"
#include "HashFunctions.h"
#include "config.h"


cuckoo_N::cuckoo_N(const UserConfig& user)
{
	F_ROW = user.ning_filter_row;
	F_COL = user.ning_filter_col;
	ratio = (double)(user.ning_ratio * F_ROW * F_COL) / 100;
	R_count = 0;
	T1_ROW = user.ning_T1_row;
	T1_COL = user.ning_T1_col;
	FT = user.ning_ft - 1;

	//Filter
	Filter = new FBucket * [F_ROW];
	for (ULONG i = 0; i < F_ROW; i++) {
		Filter[i] = new FBucket[F_COL];
	}
	//recordLayer
	T1 = new Entry * [T1_ROW];
	WCount = new long[T1_ROW];
	for (ULONG i = 0; i < T1_ROW; i++) {
		T1[i] = new Entry[T1_COL];
		WCount[i] = 0;
	}

	//test
	Log::create(logPathTest);
	Log::addTest("选择值："+to_string(user.m_record)+ "\t过滤值：" + to_string(user.ning_ft));
	Log::create(logPath_updataFilter);
}

cuckoo_N::~cuckoo_N()
{
	//LogTest();
	for (ULONG i = 0; i < F_ROW; i++) {
		delete[] Filter[i];
	}
	delete[] Filter;

	for (ULONG i = 0; i < T1_ROW; i++) {
		delete[] T1[i];
	}
	delete[] T1;
	
}

void cuckoo_N::insert(const FlowID& fid) {
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
		if (Filter[0][index[0]].count == Filter[1][index[1]].count && Filter[1][index[1]].count == Filter[2][index[2]].count ) {
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
		else  if(F_row != ULONG_MAX){
			++Filter[F_row][F_col].count;
		}
	}
	if (R_count >= ratio) updataFilter();
	if (pass)  insert_identifier(fid);
}

bool cuckoo_N::updataFilter() {
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

void cuckoo_N::getFlowPosition_filter(const FlowID& fid, ULONG* index)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	index[0] = RS(buf, FID_LEN) % F_COL;
	index[1] = SBox(buf, FID_LEN) % F_COL;
	index[2] = FNV1(buf, FID_LEN) % F_COL;
}

bool cuckoo_N::insert_identifier(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG sign = OAAT(buf, FID_LEN);//指纹
	ULONG row1 = BOB(buf, FID_LEN) % T1_ROW;//位置一
	Entry e(sign);//新建一个条目
	//检查是否已记录，如没有记录且有空位，则加入
	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row1][i].sign == e.sign) {		//指纹相同，计数器加一
			++T1[row1][i].count;
			return TRUE;
		}
		if (T1[row1][i].IsEmpty()) {	//遇见第一个空位，直接插入
			T1[row1][i] = e;
			return TRUE;
		}
	}
	//备用位置
	ULONG row2 = row1 ^ sign % T1_ROW;
	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row2][i].sign == e.sign) {	//指纹相同，计数器加一
			++T1[row2][i].count;
			return TRUE;
		}
		if (T1[row2][i].IsEmpty()) {	//遇见第一个空位，直接插入
			T1[row2][i] = e;
			return TRUE;
		}
	}

	//桶已满，且未被记录
	int sx;
	if (TWMX(buf, FID_LEN) % 2 == 0)
		sx = -1;
	else sx = 1;	//波浪计数器的1，-1值
	ULONG index_row, index_col, min = ULONG_MAX;
	for (ULONG i = 0; i < T1_COL; i++) {
		long fi = WCount[row1] * sx;
		if (T1[row1][i].count < fi) {
			if (min > T1[row1][i].count) {
				min = T1[row1][i].count;
				index_row = row1;
				index_col = i;
			}
		}
	}
	//备用位置
	for (ULONG i = 0; i < T1_COL; i++) {
		long fi = WCount[row2] * sx;
		if (fi > T1[row2][i].count) {
			if (min > T1[row2][i].count) {
				min = T1[row2][i].count;
				index_row = row2;
				index_col = i;
			}
		}
	}

	int se;
	((FlowID*)&fid)->ToData(buf);
	if (TWMX(buf, FID_LEN) % 2 == 0)
		se = -1;
	else se = 1;

	if (min != ULONG_MAX) {
		if (T1[index_row][index_col].flags)
			WCount[index_row] += (T1[index_row][index_col].count * se);
		else WCount[index_row] += se;

		T1[index_row][index_col].count++;
		T1[index_row][index_col].flags = FALSE;
		T1[index_row][index_col].sign = sign;
	}
	else {
		WCount[row1] += sx;
		WCount[row2] += sx;
	}
	return FALSE;
}



ULONG cuckoo_N::getFlowNum(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG sign = OAAT(buf, FID_LEN);
	ULONG row1 = BOB(buf, FID_LEN) % T1_ROW;

	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row1][i].sign == sign)
			return T1[row1][i].count;
	}
	ULONG row2 = row1 ^ sign % T1_ROW;
	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row2][i].sign == sign)
			return T1[row2][i].count;
	}
	return 0;
}

void cuckoo_N::LogTest() {

	Log::addTest(to_string(ratio) + "\t" + to_string(R_count));
	Log::addTest_endl();
	for (ULONG i = 0; i < T1_ROW; i++) {
		for (ULONG j = 0; j < T1_COL; j++) {
			Log::addTest(to_string(WCount[i]) + "\t" + to_string(T1[i][j].count) + "\t");
		}
		Log::addTest_endl();
	}
	Log::add_updataFilter("更新阈值：" + to_string(ratio) + "\t已满个数：" + to_string(R_count) + "\t更新次数：" + to_string(updata));
	Log::add_updataFilter("LogTest：");
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}
}

