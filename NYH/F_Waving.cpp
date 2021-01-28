#include "pch.h"
#include "F_Waving.h"
#include "HashFunctions.h"
#include "config.h"


F_Waving::F_Waving(const UserConfig& user)
{
	F_ROW = user.ning_filter_row;
	F_COL = user.ning_filter_col;
	ratio = (double)(user.ning_ratio * F_ROW * F_COL) / 100;
	R_count = 0;

	ROW = user.ning_T3_row;
	COL = user.ning_T3_col;
	FT = user.ning_ft - 1;


	//Filter
	Filter = new FBucket * [F_ROW];
	for (ULONG i = 0; i < F_ROW; i++) {
		Filter[i] = new FBucket[F_COL];
	}
	//recordLayer
	WS = new Entry * [ROW];
	ws_count = new long[ROW];
	for (ULONG i = 0; i < ROW; i++) {
		WS[i] = new Entry[COL];
		ws_count[i] = 0;
	}

	//test
	Log::create(logPathTest);
	Log::addTest("选择值：" + to_string(user.m_record));
}

F_Waving::~F_Waving()
{
	for (ULONG i = 0; i < F_ROW; i++) {
		delete[] Filter[i];
	}
	delete[] Filter;

	for (ULONG i = 0; i < ROW; i++) {
		delete[] WS[i];
	}
	delete[] WS;

}

void F_Waving::insert(const FlowID& fid) {
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
bool F_Waving::updataFilter() {
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

void F_Waving::getFlowPosition_filter(const FlowID& fid, ULONG* index)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	index[0] = RS(buf, FID_LEN) % F_COL;
	index[1] = SBox(buf, FID_LEN) % F_COL;
	index[2] = FNV1(buf, FID_LEN) % F_COL;
}

void F_Waving::insert_identifier(const FlowID& fid) {
	int sx;
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG row = BOB(buf, FID_LEN) % ROW;
	if (TWMX(buf, FID_LEN) % 2 == 0)
		sx = -1;
	else sx = 1;
	for (ULONG i = 0; i < COL; i++) {
		if (WS[row][i].fid == fid) {
			WS[row][i].count++;
			return;
		}

		if (WS[row][i].isEmpty()) {
			WS[row][i].fid = fid;
			WS[row][i].count = 1;
			WS[row][i].flags = true;
			return;
		}
	}

	long min = ULONG_MAX, col;
	for (ULONG i = 0; i < COL; i++) {
		if (min > WS[row][i].count) {
			min = WS[row][i].count;
			col = i;
		}
	}
	long fi = ws_count[row] * sx;

	if (fi >= min) {
		int se;
		((FlowID*)&WS[row][col].fid)->ToData(buf);
		if (TWMX(buf, FID_LEN) % 2 == 0)
			se = -1;
		else se = 1;

		if (WS[row][col].flags)
			ws_count[row] += (WS[row][col].count * se);
		else ws_count[row] += se;

		WS[row][col].count++;
		WS[row][col].flags = false;
		WS[row][col].fid = fid;
	}
	else ws_count[row] += sx;
}



ULONG F_Waving::getFlowNum(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG row = BOB(buf, FID_LEN) % ROW;
	for (ULONG i = 0; i < COL; i++) {
		if (WS[row][i].fid == fid)
			return WS[row][i].count;
	}
	return 0;
}

void F_Waving::LogTest() {
	Log::create(logPathTest);
	for (ULONG i = 0; i < ROW; i++) {
		Log::addTest_ws(to_string(ws_count[i]) + "\t");
		for (ULONG j = 0; j < COL; j++) {
			Log::addTest_ws(to_string(WS[i][j].count) + "\t");
		}
		Log::add_endl();
	}
}

