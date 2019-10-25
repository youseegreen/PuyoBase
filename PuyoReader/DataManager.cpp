#include "DataManager.h"
#include <stdio.h>
#include <iostream>
#pragma warning (disable:4996)
using namespace std;
using namespace cv;


//今のフィールドと次のフィールドから連鎖やお邪魔を確認する
bool DataInputor::IsChainOrOjama() {
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			if (data.nextField[x][y] != data.field[x][y]) {
				if (data.field[x][y] != N) return true;
			}
		}
	}
	return false;
}

//正解の手を探し出す
void DataInputor::GetCorrect() {
	int p1[2], p2[2];
	int differentNum = 0;
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			if (data.nextField[x][y] != data.field[x][y]) {
				differentNum++;
				if (differentNum == 1) { p1[0] = x; p1[1] = y; }
				else { p2[0] = x; p2[1] = y; }
			}
		}
	}

	//ぷよ数一致しないならUnknownでend
	if (differentNum != 2) {
		data.correctNum = Unk;
		return;
	}

	int tumo1x, tumo1y, tumo2x, tumo2y;
	if (data.nextField[p1[0]][p1[1]] == data.now[0]) {
		tumo1x = p1[0], tumo1y = p1[1];
		tumo2x = p2[0], tumo2y = p2[1];
	}
	else if (data.nextField[p2[0]][p2[1]] == data.now[0]) {
		tumo2x = p1[0], tumo2y = p1[1];
		tumo1x = p2[0], tumo1y = p2[1];
	}
	else {
		data.correctNum = Unk;
		return;
	}
	if (tumo1x == tumo2x) {
		if (tumo1y > tumo2y)data.correctNum = Method(tumo1x);
		else data.correctNum = Method(tumo1x + 6);
	}
	else {
		if (tumo1x == (tumo2x - 1))data.correctNum = Method(tumo1x + 12);
		else if ((tumo1x - 1) == tumo2x)data.correctNum = Method(tumo2x + 17);
		else data.correctNum = Unk;
	}
}

void DataInputor::WriteFile(const char *fName) {
	FILE *fp = fopen(fName, "w");
//	printf("%lf\n", data.start2NowTime);
	fprintf(fp, "%lf\n", data.start2NowTime);
	fprintf(fp, "%d\n", (int)data.correctNum);
	fprintf(fp, "%d,%d\n", (int)data.now[0], (int)data.now[1]);
	fprintf(fp, "%d,%d\n", (int)data.next[0][0], (int)data.next[0][1]);
	fprintf(fp, "%d,%d\n", (int)data.next[1][0], (int)data.next[1][1]);
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			fprintf(fp, "%d,", (int)data.field[x][y]);
		}
		fprintf(fp, "\n");
	}

	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			fprintf(fp, "%d,", (int)data.nextField[x][y]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
}

void DataInputor::ReadFile(const char *fName) {
	FILE *fp = fopen(fName, "r");
	if (fp == NULL) {
		printf("file open error\n"); getchar();
	}
	fscanf(fp, "%lf\n", &data.start2NowTime);
	fscanf(fp, "%d\n", &data.correctNum);
	fscanf(fp, "%d,%d\n", &data.now[0], &data.now[1]);
	fscanf(fp, "%d,%d\n", &data.next[0][0], &data.next[0][1]);
	fscanf(fp, "%d,%d\n", &data.next[1][0], &data.next[1][1]);
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			fscanf(fp, "%d,", &data.field[x][y]);
		}
		fscanf(fp, "\n");
	}
	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			fscanf(fp, "%d,", &data.nextField[x][y]);
		}
		fscanf(fp, "\n");
	}
	fclose(fp);
}


//5色→4色へ
void DataInputor::Color5To4() {
	if (data.correctNum == Unk)return;
	bool frag[7] = { false };

	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			frag[(int)data.field[x][y]] = true;
			frag[(int)data.nextField[x][y]] = true;
		}
	}
	for (int j = 0; j < 2; j++) {
		frag[(int)data.now[j]] = true;
		frag[(int)data.next[0][j]] = true;
		frag[(int)data.next[1][j]] = true;
	}

	int num[7] = { 0,1,2,3,4,10,5 };

	if (frag[5] == true) {
		if (frag[4] == false)num[5] = 4;
		else if (frag[3] == false)num[5] = 3;
		else if (frag[2] == false)num[5] = 2;
		else if (frag[1] == false)num[5] = 1;
		else data.correctNum = Unk;
	}

	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			data.field[x][y] = (Color)num[(int)data.field[x][y]];
			data.nextField[x][y] = (Color)num[(int)data.nextField[x][y]];
		}
	}
	for (int j = 0; j < 2; j++) {
		data.now[j] = (Color)num[(int)data.now[j]];
		data.next[0][j] = (Color)num[(int)data.next[0][j]];
		data.next[1][j] = (Color)num[(int)data.next[1][j]];
	}
}
