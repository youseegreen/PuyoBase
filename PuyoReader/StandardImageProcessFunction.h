#pragma once
#include "OpenCV.h"
#include "GetFileNameFromFolder.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
using namespace cv;

//フィールドの枠線
struct FieldBorders {
	int right;
	int left;
	int down;
	int up;
};

struct Circle {
	cv::Point center;
	double r;
};

//videoの基本情報
void GetVideoInfo(cv::VideoCapture *video, int *count = NULL, double *fps = NULL, int *width = NULL, int *height = NULL);
//黒塗り画像か判定する
bool IsBlackImage(Mat *image);
//2枚の画像の誤差を返す
int Caluculate2ImageError(Mat *in1, Mat *in2);
bool RThita(Mat *input, Point Gp, FieldBorders *cor, int wid, int hei);
//videoをcountだけ進める
void FowardVideoFrame(VideoCapture *input, Mat *frame, int count);
//平均と標準偏差を求める
void GetRGBMeanAndStandardDeviation(Mat *input, double mean[3], double dis[3]);
//ベクトルのノルム求めるだけ
double GetDistance(double *d1, double*d2, int dim);
//配列内の最小見つけるだけ
void SearchMinimum(double *value, int num, double &minV, int &min);


//K平均法　入力ファイル名、データ数、入力は何次元？、何データに分類？、格納データ
void KMeans(char *fName, int dataNum, int dim, int k, char *outputName);

void KMeans2(const char *fName, int dataNum, int dim, int k, const char *outputName, int exceptNum = 0);

