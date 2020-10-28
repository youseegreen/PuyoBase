#pragma once
#include "PuyoPlayer.h"

using namespace cv;
using namespace std;

class OneVideoPuyoPlayer {
	PuyoPlayer pp;
	int allGameNum;
	string rootFolderName;
	vector<string> eachGameFolderNameList;

	int winWidth, winHeight;
	int puyoSize;
	string windowName;
	bool is1p;
	int allMoveTimes;
	float time;
	float nbaisoku;

public:
	OneVideoPuyoPlayer(string fName) {
		rootFolderName = fName;
		eachGameFolderNameList = Directory::GetDirectoryNameList(rootFolderName.c_str(), regex("(\\d+)"));
		allGameNum = (int)(eachGameFolderNameList.size());
		cout << allGameNum << endl;
		for (int i = 0; i < allGameNum; i++)cout << eachGameFolderNameList[i] << endl;
		windowName = "game";
		puyoSize = 50;
		winWidth = puyoSize * 8;
		winHeight = puyoSize * 14;
	}


	void Play(int n = 0) {
		for (int i = n; i < allGameNum; i++) {
			pp.ReadFolder((rootFolderName + string("\\") + eachGameFolderNameList[i] + string("\\")).c_str());
			pp.Play();
		}
	}

};
