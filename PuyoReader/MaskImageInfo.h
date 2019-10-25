#pragma once
#include "OpenCV.h"
//#include "PuyoImageProcessFunction.h"
#include "StandardImageProcessFunction.h"
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>


using namespace cv;
using namespace std;

#define EOL 1000000	



class MaskImageInfoOnePlayer {
	int player;
	FieldBorders fieldBorders;
	Circle nex1;
	Circle nex2;
	Circle nexnex1;
	Circle nexnex2;

	Rect deathJudge;	//?
	Mat src;
	double deathValue[3];	//?
	double width;	//?
	double height;	//?
	bool isMaskInfoSetted;

public:
	void Initialize(int playerNum) { 
		player = playerNum; 
		for (int i = 0; i < 3; i++)deathValue[i] = 0; 
		isMaskInfoSetted = false;
	}

	bool SetMaskCorner(FieldBorders input) { 
		if (isMaskInfoSetted)return false;
		fieldBorders = input;
		isMaskInfoSetted = true;
		return true;
	}

	void CaluculateMaskImage(cv::Mat *input);
	const void GetSrcDifferentImage(cv::Mat *output, const cv::Mat *input);
	void GetMaskImageField(cv::Mat *maskImg, int x, int y, const cv::Mat *ref = NULL);
	void GetMaskImageCircle(cv::Mat *maskImg, int next, int num, const cv::Mat *ref = NULL);
	void GetMaskImagePoint(cv::Mat *maskImg, const cv::Mat *ref);
	void ForDebug(char *fName);
	bool IsGameFinished(cv::Mat *nowFrame);
};


class MaskImageInfo {
private:
	MaskImageInfoOnePlayer p1;
	MaskImageInfoOnePlayer p2;

	void CaluculateMaskImage(cv::Mat *input) { p1.CaluculateMaskImage(input); p2.CaluculateMaskImage(input); }

public:
	MaskImageInfo() { p1.Initialize(0); p2.Initialize(1); }
	bool GetMaskInfo(VideoCapture *video, Mat *nowFrame);
	bool IsStartFrame(Mat *nowFrame);
	void const GetSrcDifferentImage(cv::Mat *output, int player, const cv::Mat *input);
	void GetMaskImageField(cv::Mat *maskImg, int player, int x, int y, const cv::Mat *ref = NULL);
	void GetMaskImageCircle(cv::Mat *maskImg, int player, int next, int num, const cv::Mat *ref = NULL);
	void GetMaskImagePoint(cv::Mat *maskImg, int player, const cv::Mat *ref);
	void ForDebug(const char *fName);
	int IsGameFinished(cv::Mat *nowFrame) { if (p1.IsGameFinished(nowFrame))return 1; if (p2.IsGameFinished(nowFrame)) return 0; return -1; }
};