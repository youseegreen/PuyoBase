#pragma once
#include "OpenCV.h"
#include <stdio.h>
#include <iostream>
#include "MaskImageInfo.h"

using namespace cv;

enum Color {
	N, R, G, B, Y, P, O, Fail = 7
};

enum Method {
	U1, U2, U3, U4, U5, U6, D1, D2, D3, D4, D5, D6, R1, R2, R3, R4, R5, L2, L3, L4, L5, L6, Unk
};

struct DataFormat {
	Color field[6][12];
	Color nextField[6][12];
	Color next[2][2];
	Color now[2];
	Method correctNum;		//intをenumにする
	double start2NowTime;	//ゲーム開始からか動画のフレームかは未定
	DataFormat() {
		for(int y = 0;y<12;y++){
			for (int x = 0; x < 6; x++) {
				field[x][y] = N;
				nextField[x][y] = N;
			}
		}
		for (int i = 0; i < 2; i++) {
			now[i] = N;
			next[i][0] = N;
			next[i][1] = N;
		}
		correctNum = Unk;
		start2NowTime = 0;
	}
};

class DataInputor {
	DataFormat data;
public:
	void InputFile() {	}
	void WriteFile(const char *fName);
	void ReadFile(const char *fName);

	//getter, setter
	int GetCorrectNum() { return (int)data.correctNum; }
	void InputField(int x, int y, Color c) { data.field[x][y] = c; }
	void InputNextField(int x, int y, Color c) { data.nextField[x][y] = c; }
	void InputNext(int nex, int n, Color c) { data.next[nex][n] = c; }
	void InputNow(int n, Color c) { data.now[n] = c; }
	void SetTime(const int nowFrame, const int startFrame, const double fps) {
		data.start2NowTime = (nowFrame - startFrame) / fps;
		printf("%d, %d, %lf, %lf\n", nowFrame, startFrame, fps, data.start2NowTime);
	}
	bool IsChainOrOjama();	//今のフィールドと次のフィールドから連鎖やお邪魔を確認する
	void GetCorrect();	//正解の手を探し出す
	void Color5To4();
	bool IsUnk() { if (data.correctNum == Unk)return true; return false; }

	void Debug() {
		printf("\nデータ\n");
		printf("時間：%lf\n", data.start2NowTime);
		printf("次の手：%d\n", (int)data.correctNum);
		printf("N：0, R：1, G：2, B：3, Y：4, P：5, O：6, Fail：7\n");
		printf("現在手：%d, %d\n", (int)data.now[0], (int)data.now[1]);
		printf("ネクスト：%d, %d\n", (int)data.next[0][0], (int)data.next[0][1]);
		printf("ネクネク：%d, %d\n", (int)data.next[1][0], (int)data.next[1][1]);
		printf("フィールド\n");
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				printf("%d, ", (int)data.field[x][y]);
			}
			printf("\n");
		}
		printf("次のフィールド\n");
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				printf("%d, ", (int)data.nextField[x][y]);
			}
			printf("\n");
		}
		printf("\n");
	}
//	void InputData2double4DeepLearning(double *inputData, double *outputData, int change);	//要修正
//	void InputData2double4EvaluationFunction(double *inputData, double *outputData);	//要修正
};



