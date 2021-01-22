#include "pch.h"
#include "F_Elastic.h"
#include "pch.h"
#include "HashFunctions.h"
#include "config.h"


F_Elastic::F_Elastic(const UserConfig& user)
{
	F_ROW = user.ning_filter_row;
	F_COL = user.ning_filter_col;
	ratio = (double)(user.ning_ratio * F_ROW * F_COL) / 100;
	R_count = 0;

	ROW_NUM  = user.ning_T2_row;
	COL_NUM  = user.ning_T2_col;
	FT = user.ning_ft - 1;

	voteThreshold = user.ning_kickLimit;
	
	//Filter
	Filter = new FBucket * [F_ROW];
	for (ULONG i = 0; i < F_ROW; i++) {
		Filter[i] = new FBucket[F_COL];
	}
	//recordLayer
	entryTable = new Entry * [ROW_NUM];
	fVotes = new ULONG[ROW_NUM];
	for (ULONG i = 0; i < ROW_NUM; i++) {
		entryTable[i] = new Entry[COL_NUM];
		fVotes[i] = 0;
	}

	//test
	Log::create(logPathTest);
	Log::create(logPath_updataFilter);
}

F_Elastic::~F_Elastic()
{
	for (ULONG i = 0; i < F_ROW; i++) {
		delete[] Filter[i];
	}
	delete[] Filter;

	for (ULONG i = 0; i < COL_NUM; i++) {
		delete[] entryTable[i];
	}
	delete[] entryTable;

}

void F_Elastic::insert(const FlowID& fid) {
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

bool F_Elastic::updataFilter() {
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

void F_Elastic::getFlowPosition_filter(const FlowID& fid, ULONG* index)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	index[0] = RS(buf, FID_LEN) % F_COL;
	index[1] = SBox(buf, FID_LEN) % F_COL;
	index[2] = FNV1(buf, FID_LEN) % F_COL;
}

bool F_Elastic::insert_identifier(const FlowID& fid) {
	Pair<ULONG, ULONG> position(ULONG_MAX, ULONG_MAX);
	getFlowPosition(fid, position);
	ULONG index = position.k;
	ULONG col = position.v;

	// 检查当前流是否已经存在，如果已经存在，则直接正票数+1
	if (col != ULONG_MAX) {
		entryTable[index][col].pVote++;
		return true;
	}

	// 不存在，则判断当前表是否存在空位置
	for (ULONG i = 0; i < COL_NUM; i++) {
		if (entryTable[index][i].isEmpty()) {
			entryTable[index][i].fid = fid;
			entryTable[index][i].pVote = 1;
			return true;
		}
	}

	// 不存在空位置，反票+1
	fVotes[index] ++;

	// 清空小于阈值的槽
	checkAndReset(index);

	// 将新流加入
	for (ULONG i = 0; i < COL_NUM; i++) {
		if (entryTable[index][i].isEmpty()) {
			entryTable[index][i].fid = fid;
			entryTable[index][i].pVote = 1;
			return true;
		}
	}
	
	return false;
}

bool F_Elastic::checkAndReset(ULONG index)
{
	for (ULONG i = 0; i < COL_NUM; i++) {
		if (entryTable[index][i].pVote > 0 && (double)fVotes[index] / entryTable[index][i].pVote > voteThreshold) {
			entryTable[index][i].reset();
			fVotes[index] = 0;
			return true;
		}
	}
	return true;
}

ULONG F_Elastic::getFlowNum(const FlowID& fid)
{
	Pair<ULONG, ULONG> position(ULONG_MAX, ULONG_MAX);
	getFlowPosition(fid, position);
	if (position.v != ULONG_MAX) {
		return entryTable[position.k][position.v].pVote;
	}
	return 0;
}

void F_Elastic::getFlowPosition(const FlowID& fid, Pair<ULONG, ULONG>& pair)
{
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG index = BOB(buf, FID_LEN) % ROW_NUM;
	pair.k = index;
	for (ULONG i = 0; i < COL_NUM; i++) {
		if (entryTable[index][i].fid == fid) {
			pair.v = i;
			break;
		}
	}
}

void F_Elastic::LogTest() {
	Log::create(logPathTest);
	Log::addTest(to_string(ratio) + "\t" + to_string(R_count));
	Log::addTest_endl();
	for (ULONG i = 0; i < ROW_NUM; i++) {
		Log::addTest(to_string(fVotes[i])+ "\t");
		for (ULONG j = 0; j < COL_NUM; j++) {
			Log::addTest(to_string(entryTable[i][j].pVote) + "\t");
		}
		Log::addTest_endl();
	}
	Log::add_updataFilter("LogTest：");
	for (ULONG i = 0; i < F_COL; i++) {
		Log::add_updataFilter(to_string(Filter[0][i].count) + "\t" + to_string(Filter[0][i].flags) + "\t" + to_string(Filter[1][i].count) + "\t" + to_string(Filter[1][i].flags) + "\t" + to_string(Filter[2][i].count) + "\t" + to_string(Filter[2][i].flags));
	}
}

