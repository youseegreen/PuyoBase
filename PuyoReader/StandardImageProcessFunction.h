#pragma once
#include "OpenCV.h"
#include "GetFileNameFromFolder.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
using namespace cv;

//�t�B�[���h�̘g��
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

//video�̊�{���
void GetVideoInfo(cv::VideoCapture *video, int *count = NULL, double *fps = NULL, int *width = NULL, int *height = NULL);
//���h��摜�����肷��
bool IsBlackImage(Mat *image);
//2���̉摜�̌덷��Ԃ�
int Caluculate2ImageError(Mat *in1, Mat *in2);
bool RThita(Mat *input, Point Gp, FieldBorders *cor, int wid, int hei);
//video��count�����i�߂�
void FowardVideoFrame(VideoCapture *input, Mat *frame, int count);
//���ςƕW���΍������߂�
void GetRGBMeanAndStandardDeviation(Mat *input, double mean[3], double dis[3]);
//�x�N�g���̃m�������߂邾��
double GetDistance(double *d1, double*d2, int dim);
//�z����̍ŏ������邾��
void SearchMinimum(double *value, int num, double &minV, int &min);


//K���ϖ@�@���̓t�@�C�����A�f�[�^���A���͉͂������H�A���f�[�^�ɕ��ށH�A�i�[�f�[�^
void KMeans(char *fName, int dataNum, int dim, int k, char *outputName);

void KMeans2(const char *fName, int dataNum, int dim, int k, const char *outputName, int exceptNum = 0);

