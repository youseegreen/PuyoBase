#pragma once
#include <string>
#include <time.h>
#include "../PuyoReader/OpenCV.h"
#include "../PuyoReader/GetFileNameFromFolder.h"

using namespace cv;
using namespace std;

class PuyoPlayer {
private:
	enum PuyoColor {
		N, R, G, B, Y, P, O, Fail = 7
	};
	enum Method {
		U1, U2, U3, U4, U5, U6, D1, D2, D3, D4, D5, D6, R1, R2, R3, R4, R5, L2, L3, L4, L5, L6, Unk
	};

	struct Data {
		PuyoColor field[6][12];
		PuyoColor nextField[6][12];
		PuyoColor next[2][2];
		PuyoColor now[2];
		Method correctNum;		//intをenumにする
		double start2NowTime;	//ゲーム開始からか動画のフレームかは未定
	} data;

	int winWidth, winHeight;
	int puyoSize;
	string windowName;
	string folderName;
	bool is1p;
	int allMoveTimes;
	float time;
	float nbaisoku;

	bool ReadFile(string csvName) {
		FILE *fp;
		fopen_s(&fp, csvName.c_str(), "r");
		if (fp == NULL) {
			printf("file open error\n"); getchar();
			return false;
		}
		fscanf_s(fp, "%lf\n", &data.start2NowTime);
		fscanf_s(fp, "%d\n", &data.correctNum);
		fscanf_s(fp, "%d,%d\n", &data.now[0], &data.now[1]);
		fscanf_s(fp, "%d,%d\n", &data.next[0][0], &data.next[0][1]);
		fscanf_s(fp, "%d,%d\n", &data.next[1][0], &data.next[1][1]);
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				fscanf_s(fp, "%d,", &data.field[x][y]);
			}
			fscanf_s(fp, "\n");
		}
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				fscanf_s(fp, "%d,", &data.nextField[x][y]);
			}
			fscanf_s(fp, "\n");
		}
		fclose(fp);
		return true;
	}

	Scalar GetColor(PuyoColor col) {
		switch (col)
		{
		case PuyoPlayer::N:
			return Scalar(0, 0, 0);
		case PuyoPlayer::R:
			return Scalar(0, 0, 255);
		case PuyoPlayer::G:
			return Scalar(0, 255, 0);
		case PuyoPlayer::B:
			return Scalar(255, 0, 0);
		case PuyoPlayer::Y:
			return Scalar(0, 255, 255);
		case PuyoPlayer::P:
			return Scalar(255, 0, 180);
		case PuyoPlayer::O:
			return Scalar(255, 255, 255);
		case PuyoPlayer::Fail:
		default:
			return Scalar(100, 100, 100);
		}
	}

public:
	PuyoPlayer(const char *fName) {
		folderName = fName;
		vector<string> fileNameList = Directory::GetFileNameList(folderName, ".csv");
		allMoveTimes = (int)(fileNameList.size());
		windowName = "game";
		puyoSize = 50;
		winWidth = puyoSize * 8;
		winHeight = puyoSize * 14;
	}

	PuyoPlayer() {
		windowName = "game";
		puyoSize = 50;
		winWidth = puyoSize * 8;
		winHeight = puyoSize * 14;
		nbaisoku = 1;
	}

	void ReadFolder(string fName) {
		folderName = fName;
		vector<string> fileNameList = Directory::GetFileNameList(folderName, ".csv");
		allMoveTimes = (int)(fileNameList.size());
	}



	void Setting() {

	}


	void Play() {
		const clock_t startTime = clock();
		printf("reset   %d\n", startTime);
		//	waitKey(0);
		double time = 0;

		//		for (int i = 0; i < allMoveTimes; i++) {
		for (int i = 1; i < allMoveTimes; i++) {

			cout << folderName + to_string(i) + ".csv" << endl;
			if (!ReadFile((folderName + to_string(i) + ".csv").c_str())) {
				printf("error\n");
				return;
			}
			Mat src(Size(winWidth, winHeight), CV_8UC3, Scalar(0, 0, 0));	//黒画像

			//描画
			Scalar color = GetColor(data.now[0]);
			int v = 0 * puyoSize + puyoSize / 2;
			int u = 2 * puyoSize + puyoSize / 2;
			circle(src, Point(u, v), puyoSize / 2, color, -1);
			for (int i = 0; i < 2; i++) {
				Scalar color = GetColor(data.now[i]);
				int v = 0 * puyoSize + puyoSize / 2;
				int u = (2 + i) * puyoSize + puyoSize / 2;
				circle(src, Point(u, v), puyoSize / 2, color, -1);
				for (int j = 0; j < 2; j++) {
					color = GetColor(data.next[i][j]);
					v = (2 + j)*puyoSize + i * 3 * puyoSize + puyoSize / 2;
					u = 7 * puyoSize + puyoSize / 2;
					circle(src, Point(u, v), puyoSize / 2, color, -1);
				}
			}


			for (int y = 0; y < 12; y++) {
				for (int x = 0; x < 6; x++) {
					int v = y * puyoSize + puyoSize / 2 + 2 * puyoSize;
					int u = x * puyoSize + puyoSize / 2;

					Scalar color = GetColor(data.field[x][y]);
					circle(src, Point(u, v), puyoSize / 2, color, -1);
				}
			}

			imshow(windowName.c_str(), src);
			while (time < (data.start2NowTime / nbaisoku)) {
				time = double((clock() - startTime) / (double)CLOCKS_PER_SEC);
				//	printf("%lf, %lf,  %d   %lf    %d\n", data.start2NowTime / nbaisoku, time, clock(), startTime, clock()-startTime);
			}
			waitKey(1);
		}
	}
};