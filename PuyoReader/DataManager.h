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
	Method correctNum;		//int��enum�ɂ���
	double start2NowTime;	//�Q�[���J�n���炩����̃t���[�����͖���
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
	bool IsChainOrOjama();	//���̃t�B�[���h�Ǝ��̃t�B�[���h����A���₨�ז����m�F����
	void GetCorrect();	//�����̎��T���o��
	void Color5To4();
	bool IsUnk() { if (data.correctNum == Unk)return true; return false; }

	void Debug() {
		printf("\n�f�[�^\n");
		printf("���ԁF%lf\n", data.start2NowTime);
		printf("���̎�F%d\n", (int)data.correctNum);
		printf("N�F0, R�F1, G�F2, B�F3, Y�F4, P�F5, O�F6, Fail�F7\n");
		printf("���ݎ�F%d, %d\n", (int)data.now[0], (int)data.now[1]);
		printf("�l�N�X�g�F%d, %d\n", (int)data.next[0][0], (int)data.next[0][1]);
		printf("�l�N�l�N�F%d, %d\n", (int)data.next[1][0], (int)data.next[1][1]);
		printf("�t�B�[���h\n");
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				printf("%d, ", (int)data.field[x][y]);
			}
			printf("\n");
		}
		printf("���̃t�B�[���h\n");
		for (int y = 0; y < 12; y++) {
			for (int x = 0; x < 6; x++) {
				printf("%d, ", (int)data.nextField[x][y]);
			}
			printf("\n");
		}
		printf("\n");
	}
//	void InputData2double4DeepLearning(double *inputData, double *outputData, int change);	//�v�C��
//	void InputData2double4EvaluationFunction(double *inputData, double *outputData);	//�v�C��
};



