#include "pch.h"
#include "WavingSketch.h"
#include "HashFunctions.h"
#include "config.h"


WavingSketch::WavingSketch(const UserConfig& user)
{
	ROW = user.WS_row;
	COL = user.WS_col;

	//recordLayer
	WS = new Entry * [ROW];
	ws_count = new long[ROW];
	for (ULONG i = 0; i < ROW; i++) {
		WS[i] = new Entry[COL];
		ws_count[i] = 0;
	}
	
	//test
	Log::create(logPathTest);
	Log::create(logPath_updataFilter);
}

WavingSketch::~WavingSketch()
{

	for (ULONG i = 0; i < ROW; i++) {
		delete[] WS[i];
	}
	delete[] WS;
	delete[] ws_count;
}

void WavingSketch::insert(const FlowID& fid) {
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


ULONG WavingSketch::getFlowNum(const FlowID& fid) {
	UCHAR buf[FID_LEN];
	((FlowID*)&fid)->ToData(buf);
	ULONG row = BOB(buf, FID_LEN) % ROW;
	for (ULONG i = 0; i < COL; i++) {
		if (WS[row][i].fid == fid)
			return WS[row][i].count;
	}
	return 0;
}

void WavingSketch::LogTest() {
	Log::create(logPathTest_ws);
	for (ULONG i = 0; i < ROW; i++) {
		Log::addTest_ws(to_string(ws_count[i])+"\t");
		for (ULONG j = 0; j < COL; j++) {
			Log::addTest_ws(to_string(WS[i][j].count) + "\t");
		}
		Log::add_endl();
	}
}

