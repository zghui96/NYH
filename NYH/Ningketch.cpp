#include "pch.h"
#include "Ningketch.h"
#include "HashFunctions.h"
#include "config.h"


Ningketch::Ningketch(const UserConfig& user)
{
	F_ROW = user.ning_filter_row;
	F_COL = user.ning_filter_col;
	ratio = (double)(user.ning_ratio * F_ROW * F_COL) / 100;
	R_count = 0;
	T1_ROW = user.ning_T1_row;
	T1_COL = user.ning_T1_col;
	//T2_ROW = user.ning_T2_row;
	//T2_COL = user.ning_T2_col;
	FT = user.ning_ft - 1;

	kickLimit = user.ning_kickLimit;

	//Filter
	Filter = new FBucket * [F_ROW];
	for (ULONG i = 0; i < F_ROW; i++) {
		Filter[i] = new FBucket[F_COL];
	}
	//recordLayer
	T1 = new Entry * [T1_ROW];
	for (ULONG i = 0; i < T1_ROW; i++) {
		T1[i] = new Entry[T1_COL];
	}

	//test
	Log::create(logPathTest);
	Log::addTest("选择值：" + to_string(user.m_record));
}

Ningketch::~Ningketch()
{
	for (ULONG i = 0; i < F_ROW; i++) {
		delete[] Filter[i];
	}
	delete[] Filter;

	for (ULONG i = 0; i < T1_ROW; i++) {
		delete[] T1[i];
	}
	delete[] T1;

}

void Ningketch::insert(const FlowID& fid) {
	ULONG index[3], CU_new[3] = { FT,FT,FT };
	bool pass = TRUE;
	getFlowPosition_filter(fid, index);
	for (ULONG i = 0; i < F_ROW; i++) {
		if (!Filter[i][index[i]].flags) {
			if (Filter[i][index[i]].count < FT) {
				pass = FALSE;
			}
		}
		CU_new[i] = Filter[i][index[i]].count;
	}

	if (CU_new[0] == CU_new[1] && CU_new[2] == CU_new[1] && CU_new[0] < FT) {
		++Filter[0][index[0]].count;
		++Filter[1][index[1]].count;
		++Filter[2][index[2]].count;
		if (Filter[0][index[0]].count == FT) R_count += 3;
	}
	else {
		if (CU_new[0] > CU_new[1])
			if (CU_new[1] > CU_new[2]) {
				if (Filter[2][index[2]].count < FT) {
					++Filter[2][index[2]].count;
					if (Filter[2][index[2]].count == FT)
						++R_count;
				}
			}
			else {
				if (Filter[1][index[1]].count < FT) {
					++Filter[1][index[1]].count;
					if (Filter[1][index[1]].count == FT)
						++R_count;
				}
			}
		else {
			if (CU_new[0] > CU_new[2]) {
				if (Filter[2][index[2]].count < FT) {
					++Filter[2][index[2]].count;
					if (Filter[2][index[2]].count == FT)
						++R_count;
				}
			}
			else {
				if (Filter[0][index[0]].count < FT) {
					++Filter[0][index[0]].count;
					if (Filter[0][index[0]].count == FT)
						++R_count;
				}
			}
		}
	}
	if (R_count >= ratio) updataFilter();
	if (pass)  insert_identifier(fid);
}

bool Ningketch::updataFilter() {
	/*Log::add_updataFilter("更新前："+ to_string(ratio) + "\t" + to_string(R_count));
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}*/
	for (ULONG i = 0; i < F_ROW; i++) {
		for (ULONG j = 0; j < F_COL; j++) {
			if (Filter[i][j].count >= FT)
				Filter[i][j].flags = TRUE;
			else Filter[i][j].flags = FALSE;
			Filter[i][j].count = 0;
		}
	}
	R_count = 0;
	return FALSE;
}

void Ningketch::getFlowPosition_filter(const FlowID& fid, ULONG* index)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	index[0] = RS(buf, FID_LEN) % F_COL;
	index[1] = SBox(buf, FID_LEN) % F_COL;
	index[2] = FNV1(buf, FID_LEN) % F_COL;
}

bool Ningketch::insert_identifier(const FlowID& fid) {
	int kickNum = 0;
	bool isKick = FALSE;
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG sign = OAAT(buf, FID_LEN);
	ULONG row1 = BOB(buf, FID_LEN) % T1_ROW;
	Entry e(sign);
	//检查是否已记录，如没有记录且有空位，则加入
	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row1][i].sign == e.sign) {
			++T1[row1][i].count;
			return TRUE;
		}
		if (T1[row1][i].IsEmpty()) {
			T1[row1][i] = e;
			return TRUE;
		}
	}
	//备用位置
	ULONG row2 = row1 ^ sign % T1_ROW;
	for (ULONG i = 0; i < T1_COL; i++) {
		if (T1[row2][i].sign == e.sign) {
			++T1[row2][i].count;
			return TRUE;
		}
		if (T1[row2][i].IsEmpty()) {
			T1[row2][i] = e;
			return TRUE;
		}
	}

	//桶已满，且未被记录
	int sx;
	if (TWMX(buf, FID_LEN) % 2 == 0)
		sx = -1;
	else sx = 1;
	ULONG index_row, index_col, min = ULONG_MAX;
	for (ULONG i = 0; i < T1_COL; i++) {
		T1[row1][i].WCount += sx;
		long fi = T1[row1][i].WCount * sx;
		if (T1[row1][i].count < fi) {
			if (min > T1[row1][i].count) {
				min = T1[row1][i].count;
				index_row = row1;
				index_col = i;
				isKick = TRUE;
				//Log::addTest_endl();
				//Log::addTest( to_string(index_row) +  "\t" + to_string(index_col) + "\t" + to_string(T1[row1][i].count) + "\t");
			}
		}
	}
	//备用位置
	for (ULONG i = 0; i < T1_COL; i++) {
		T1[row2][i].WCount += sx;
		long fi = T1[row2][i].WCount * sx;
		if (fi > T1[row2][i].count) {
			if (min > T1[row1][i].count) {
				min = T1[row1][i].count;
				index_row = row2;
				index_col = i;
				isKick = TRUE;
				//Log::addTest_endl();
				//Log::addTest(to_string(index_row) + "\t" + to_string(index_col) + "\t");
			}
		}
	}
	if (isKick)  kickOut(e, index_row, index_col, kickLimit);
	
	return FALSE;
}

void Ningketch::kickOut(Entry e, ULONG row, ULONG col,ULONG kick_Num) {
	if (kick_Num > 0) {
		bool isKick = FALSE;
		Entry tmp_e = T1[row][col];
		if (kick_Num == kickLimit) {
			e.count = T1[row][col].count + 1;
			T1[row][col] = e;
		}
		else T1[row][col] = e;

		ULONG index_row, index_col, min;
		index_row = row ^ tmp_e.sign % T1_ROW;

		min = tmp_e.count;
		for (ULONG i = 0; i < T1_COL; i++) {
			if (min > T1[index_row][i].count) {
				min = T1[index_row][i].count;
				index_col = i;
				isKick = TRUE;
			}
		}
		if (isKick)  kickOut(tmp_e, index_row, index_col, --kick_Num);
	}
}

ULONG Ningketch::getFlowNum(const FlowID& fid) {
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

void Ningketch::LogTest() {
	
	Log::addTest(to_string(ratio) + "\t" + to_string(R_count));
	Log::addTest_endl();
	for (ULONG i = 0; i < T1_ROW; i++) {
		for (ULONG j = 0; j < T1_COL; j++) {
			Log::addTest(to_string(T1[i][j].WCount) + "\t" + to_string(T1[i][j].count) + "\t");
		}
		Log::addTest_endl();
	}
	Log::add_updataFilter("LogTest：");
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}
}

