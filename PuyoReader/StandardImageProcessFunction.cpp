#include "StandardImageProcessFunction.h"

//video��count�����i�߂�
void FowardVideoFrame(VideoCapture *input, Mat *frame, int count) {
	int now = (int)input->get(CV_CAP_PROP_POS_FRAMES);
	input->set(CV_CAP_PROP_POS_FRAMES, now + count - 1);
	*input >> *frame;
//	for (int i = 0; i < count; i++) *input >> *frame;
}

//video�̊�{���
void GetVideoInfo(cv::VideoCapture *video, int *count, double *fps, int *width, int *height) {
	int fourcc = (int)video->get(CV_CAP_PROP_FOURCC);
	double f = video->get(CV_CAP_PROP_FPS);
	int wid = (int)video->get(CV_CAP_PROP_FRAME_WIDTH);
	int hei = (int)video->get(CV_CAP_PROP_FRAME_HEIGHT);
	int cou = (int)video->get(CV_CAP_PROP_FRAME_COUNT);
	if (count != NULL)*count = cou;
	if (fps != NULL)*fps = f;
	if (width != NULL)*width = wid;
	if (height != NULL)*height = hei;
	printf("count %d  fps %f  width %d  height %d  ", cou, f, wid, hei);
	for (int i = 0; i < 4; i++) {
		char tmp = fourcc >> 8 * i;
		printf("%c", tmp);
	}
	printf("\n");
}

//���h��摜�����肷��
bool IsBlackImage(Mat *input) {
	int sum = 0;
	for (int y = 0; y < input->rows; y++) {
		for (int x = 0; x < input->cols; x++) {
			for (int c = 0; c < input->channels(); c++) {
				sum += input->data[y*input->step + x * input->channels() + c];
			}
		}
	}
	if (sum < 100)return true;
	return false;
}



bool RThita(Mat *input, Point Gp, FieldBorders *cor, int wid, int hei) {
	//360��ԋ߂��Ƃ�������

	//�܂��E��
	int i = 2;
	while (input->data[Gp.y * input->step + (Gp.x + i) * input->channels() + 1] != 255) {
		i++;
		if (i == wid - Gp.x - 1) return false;
	}
	//	cor->right = Gp.x + i;
	cor->right = Gp.x + i + 1;	//�����p

	//����
	i = -2;
	while (input->data[Gp.y * input->step + (Gp.x + i) * input->channels() + 1] != 255) {
		i--;
		if (i == -Gp.x) return false;
	}
	//	cor->left = Gp.x + i;
	cor->left = Gp.x + i + 1;	//�����p

	//�㑤
	i = -2;
	while (input->data[(Gp.y + i) * input->step + Gp.x * input->channels() + 1] != 255) {
		i--;
		if (i == -Gp.y) return false;
	}
	cor->up = Gp.y + i;

	//����  //��������2���
	i = 2;
	while (input->data[(Gp.y + i) * input->step + Gp.x * input->channels() + 1] != 255) {
		i++;
		if (i == hei - Gp.y - 1) return false;
	}
	double width = cor->right - cor->left;
	double height = Gp.y + i - cor->up;
	/*
	if (((height / width) > 1.53) || ((height / width) < 1.49)) {
		//�덷���傫���ꍇ�͂��̎��̐�������
		i += 2;
		while (input->data[(Gp.y + i) * input->step + Gp.x * input->channels() + 1] != 255) {
			i++;
			if (i == hei - Gp.y - 1) return false;
		}
	}*/
	cor->down = Gp.y + i;
	return true;
}




void GetRGBMeanAndStandardDeviation(Mat *input, double mean[3], double dis[3]) {
	dis[0] = dis[1] = dis[2] = 0;
	mean[0] = mean[1] = mean[2] = 0;

	int blackCount = 0;
	//����
	for (int y = 0; y < input->rows; y++) {
		for (int x = 0; x < input->cols; x++) {
			int r = input->data[y*input->step + x * input->channels() + 2];
			int g = input->data[y*input->step + x * input->channels() + 1];
			int b = input->data[y*input->step + x * input->channels() + 0];
			if (r < 80 && g < 80 && b < 80)blackCount++;
			for (int c = 0; c < input->channels(); c++) {
				mean[c] += input->data[y*input->step + x * input->channels() + c];
			}
		}
	}
	if (blackCount > input->rows * input->cols * 0.4) {
		mean[0] = mean[1] = mean[2] = 0;
		dis[0] = dis[1] = dis[2] = 0;
		return;	//���̗̈悪30%�ȏ�Ȃ烊�^�[��
	}
	for (int c = 0; c < 3; c++) {
		mean[c] /= (input->rows * input->cols - blackCount);
	}

	//�W���΍�
	for (int y = 0; y < input->rows; y++) {
		for (int x = 0; x < input->cols; x++) {
			for (int c = 0; c < input->channels(); c++) {
				if (input->data[y*input->step + x * input->channels() + 0] == 0 &&
					input->data[y*input->step + x * input->channels() + 1] == 0 &&
					input->data[y*input->step + x * input->channels() + 2] == 0) continue;
				dis[c] += pow(input->data[y*input->step + x * input->channels() + c] - mean[c], 2);
			}
		}
	}
	for (int c = 0; c < 3; c++) {
		dis[c] /= (input->rows * input->cols - blackCount);
		dis[c] = sqrt(dis[c]);
	}
}


int Caluculate2ImageError(Mat *in1, Mat *in2) {
	int sum = 0;
	if (in1->size() != in2->size()) return -1;
	for (int y = 0; y < in1->rows; y++) {
		for (int x = 0; x < in1->cols; x++) {
			for (int c = 0; c < in1->channels(); c++) {
				int t = y * in1->step + x * in1->channels() + c;
				sum += pow(in1->data[t] - in2->data[t], 2);
			}
		}
	}
	return sum;
}



void SearchMinimum(double *value, int num, double &minV, int &min) {
	min = 0;
	minV = value[0];
	for (int i = 1; i < num; i++) {
		if (minV > value[i]) {
			min = i;
			minV = value[i];
		}
		else if (minV == value[i]) {
			if ((rand() % num) == 0) min = i;
		}
	}
}

double GetDistance(double *d1, double *d2, int dim) {
	double sum = 0;
	for (int i = 0; i < dim; i++) {
	//	printf("(%lf - %lf)^2 + ", d1[i], d2[i]);
		sum += pow(d1[i] - d2[i], 2);
	}
	return sum;
}


























//Kmeans�@�ŕ���
void KMeans(const char *fName, int dataNum, int dim, int k, const char *outputName) {
	//�f�[�^�ǂݍ���
	char name[200];
	double **data = new(double *[dataNum]);
	for (int i = 0; i < dataNum; i++) {
		data[i] = new(double[dim]);
		sprintf_s(name, "%s\\test%d.png", fName, i);
		Mat img = imread(name, 1);
		double mean[3] = { 0 }, dis[3] = { 0 };
		if (!img.empty())GetRGBMeanAndStandardDeviation(&img, mean, dis);
		for (int j = 0; j < 3; j++)data[i][j] = mean[j];
		for (int j = 0; j < 3; j++)data[i][j + 3] = dis[j];
	}

	//�d�S��K���ɐݒ�(�����z�͂���)		dim��meanRGBdisRGB  k��RGBYPON
	srand((unsigned int)time(NULL));
	double **gp = new(double *[k]);
	double **nextGp = new(double *[k]);
	for (int i = 0; i < k; i++) {
		gp[i] = new(double[dim]);
		nextGp[i] = new(double[dim]);
		for (int h = 0; h < dim; h++) {
			gp[i][h] = rand() % 100;	//0~99�̗����𐶂�
		}
	}


	double *distance = new(double[k]);
	int *group = new(int[dataNum]);
	int *sum = new(int[k]);

	while (1) {
		//�ǂ̃O���[�v�ɑ�����̂��`�F�b�N
		for (int i = 0; i < dataNum; i++) {
			for (int j = 0; j < k; j++) distance[j] = GetDistance(data[i], gp[j], dim);
			int min; double minValue;
			SearchMinimum(distance, k, minValue, min);
			group[i] = min;
		}

		//���̏d�S�����߂�  �܂���0fill
		for (int i = 0; i < k; i++) {
			for (int j = 0; j < dim; j++) nextGp[i][j] = 0;
		}
		for (int j = 0; j < k; j++)sum[j] = 0;	//�ʐ�
		for (int i = 0; i < dataNum; i++) {
			sum[group[i]]++;
			for (int m = 0; m < dim; m++)	nextGp[group[i]][m] += data[i][m];
		}

		double error = 0;
		for (int j = 0; j < k; j++) {
			if (sum[j] != 0) { for (int m = 0; m < dim; m++)	nextGp[j][m] /= sum[j]; }
			else { for (int m = 0; m < dim; m++)	nextGp[j][m] = 0; }
			error += GetDistance(nextGp[j], gp[j], dim);
		}
		if (error == 0)break;
		for (int i = 0; i < k; i++) {
			for (int m = 0; m < dim; m++)gp[i][m] = nextGp[i][m];
			printf("%lf ", gp[i][0]);
		}
		printf("\n");
	}

	char fileName[100];
	sprintf_s(fileName, "kmeans_%s.csv", outputName);
	FILE *fp;
	fopen_s(&fp, fileName, "w");
	for (int i = 0; i < k; i++) {
		for (int m = 0; m < dim; m++)fprintf(fp, "%lf,", gp[i][m]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	//����
	int *number = new(int[k]);
	for (int i = 0; i < k; i++)number[i] = 0;

	for (int i = 0; i < dataNum; i++) {
		sprintf_s(name, "%s\\test%d.png", fName, i);
		Mat img = imread(name, 1);
		if (img.data != NULL) {
			sprintf_s(name, "%s%d\\test%d.png", outputName, group[i], number[group[i]]);
			imwrite(name, img);
			number[group[i]]++;
		}
	}

	//���
	for (int i = 0; i < k; i++)delete[] gp[i];
	for (int i = 0; i < k; i++)delete[] nextGp[i];
	for (int i = 0; i < dataNum; i++)delete[] data[i];
	delete[] data;
	delete[] gp;
	delete[] nextGp;
	delete[] group;
	delete[] number;
}




void PuyoKMeans(const char *fName, int dataNum, int dim, int k, const char *outputName) {
	//�t�H���_�쐬
	for (int i = 0; i < 8; i++) {
		Directory::MakeDirectory((outputName + to_string(i)).c_str());
	}

	//�f�[�^�ǂݍ���
	int colorNum[8] = { 0 };
	for (int i = 0; i < dataNum; i++) {
		string name = fName + string("\\") + to_string(i) + ".png";
		Mat img = imread(name, 1);
		if (img.empty()) { cout << "cant" << endl; continue; }

		double vote[8] = { 0 };
		for (int y = 0; y < img.rows; y++) {
			for (int x = 0; x < img.cols; x++) {
				int r = img.data[y*img.step + x * img.channels() + 2];
				int g = img.data[y*img.step + x * img.channels() + 1];
				int b = img.data[y*img.step + x * img.channels() + 0];

				if (r < 100 && g < 100 && b < 100) {
					vote[0]--;	//��
				}
				else if (g > 120 && g > r + 30 && r > b + 5) {
					vote[2]--;	//��
				}
				else if (b > g + 40 && g > r + 40) {
					vote[3]--;	//��
				}
				else if (abs(b-r) < 40 && r > g + 40) {
					vote[5]--;	//��
				}
				else if (r > 200 && r > g + 40 && abs(g - b) < 40) {
					vote[1]--;	//��
				}
				else if (r > 200 && g > b + 40) {
					vote[4]--;	//��
				}
				else if (abs(r - g) < 14 && abs(b - r) < 14 && abs(g - b) < 14) {
					vote[6]--;	//���ז�
				}
				else { 
					vote[7]--; 
				} //�s��
			}
		}
		
	//	printf("%d ", img.rows*img.cols);
	//	for (int i = 0; i < 8; i++) {
	//		printf("%lf ", vote[i]);
	//	}
		if (-vote[0] > img.rows*img.cols*0.5) vote[0] = -img.rows*img.cols;//��
		else vote[0] = 0;

		double minValue; int min;
		SearchMinimum(vote, 8, minValue, min);
		if (min == 7) {
			vote[7] = 0;
			SearchMinimum(vote, 8, minValue, min);
			if (minValue > -6)min = 7;
		}
		string outName = outputName + to_string(min) + "\\" + to_string(colorNum[min]) + ".png";
		imwrite(outName.c_str(), img);
		colorNum[min]++;
	//	printf("\n%d\n", min);
		imshow("hoge", img);
		if(min==7)waitKey(0);
	}
}

//�� R231  G=B   R>G=B
  //�� R29-100   G88-150      B215-233        R>G>B
//�� R255  G130-255  B80-128  R=255 G>B
  //�� G>R>B
//�� R=B>G   B>R>>G
//���ז� R=G=B



//		GetRGBMeanAndStandardDeviation(&img, RGBmean, RGBdis);
//		if (RGBmean[0] == 0 || RGBmean[1] == 0 || RGBmean[2] == 0) {
//			string outName = outputName + string("_black\\") + to_string(blackNum) + ".png";
//			imwrite(outName.c_str(), img);
//			blackNum++;
//		}
//	}
//	waitKey(0);









//	KMeans2(fName, dataNum, dim, k - 1, outputName, blackNum);
//}

//�� R231  G=B   R>G=B
//�� R29-100   G88-150      B215-233        R>G>B
//�� R255  G130-255  B80-128  R=255 G>B
//�� G>R>B
//�� R=B>G   B>R>>G
//���ז� R=G=B

//Kmeans�@�ŕ���
void KMeans2(const char *fName, int dataNum, int dim, int k, const char *outputName, int exceptNum) {
	//�f�[�^�ǂݍ���
	char name[200];
	double **data = new(double *[dataNum-exceptNum]);
	int tmp = 0;
	for (int i = 0; i < dataNum; i++) {
		sprintf_s(name, "%s\\%d.png", fName, i);
		Mat img = imread(name, 1);
	//	Mat dst;
		if (img.empty()) { cout << "cant" << endl; continue; }
	//	cvtColor(img, dst, cv::COLOR_BGR2HSV);
		double RGBmean[3] = { 0 }, RGBdis[3] = { 0 };	//���ςƕ��U
	//	double HSVmean[3] = { 0 }, HSVdis[3] = { 0 };	//���ςƕ��U
		GetRGBMeanAndStandardDeviation(&img, RGBmean, RGBdis);
		if (RGBmean[0] == 0 || RGBmean[1] == 0 || RGBmean[2] == 0) {
			continue;
		}
		data[tmp] = new(double[dim]);
	//	GetRGBMeanAndStandardDeviation(&dst, HSVmean, HSVdis);
		for (int j = 0; j < 3; j++)data[tmp][j] = RGBmean[j];
	//	for (int j = 0; j < 3; j++)data[tmp][j + 3] = RGBdis[j];
		tmp++;
	//	for (int j = 0; j < 3; j++)data[i][j + 6] = HSVmean[j];
	//	for (int j = 0; j < 3; j++)data[i][j + 9] = HSVdis[j];
	}
	dataNum -= exceptNum;

	//for (int j = 0; j < dim; j++) {
	//	for (int i = 0; i < dataNum; i++) {
	//		printf("data[%d][%d] %lf\n", i, j,data[i][j]);
	//		waitKey(200);
	//	}
	//	waitKey(0);
	//}


	//�d�S��K���ɐݒ�(�����z�͂���)		dim��meanRGBdisRGB  k��RGBYPON
	srand((unsigned int)time(NULL));
	double **gp = new(double *[k]);
	double **nextGp = new(double *[k]);
	for (int i = 0; i < k; i++) {
		gp[i] = new(double[dim]);
		nextGp[i] = new(double[dim]);
		int random = rand() % dataNum;
		for (int h = 0; h < dim; h++) {
		//	gp[i][h] = rand() % 100;	//0~99�̗����𐶂�
			gp[i][h] = data[random][h] + rand() % 50 - 25;
		}
	}

	double *distance = new(double[k]);
	int *group = new(int[dataNum]);
	int *sum = new(int[k]);

	while (1) {
		//�ǂ̃O���[�v�ɑ�����̂��`�F�b�N
		for (int i = 0; i < dataNum; i++) {
			for (int j = 0; j < k; j++) distance[j] = GetDistance(data[i], gp[j], dim);
			int min; double minValue;
			SearchMinimum(distance, k, minValue, min);
			group[i] = min;
		}

		//���̏d�S�����߂�  �܂���0fill
		for (int i = 0; i < k; i++) {
			for (int j = 0; j < dim; j++) nextGp[i][j] = 0;
		}
		for (int j = 0; j < k; j++)sum[j] = 0;	//�ʐ�
		for (int i = 0; i < dataNum; i++) {
			sum[group[i]]++;
			for (int m = 0; m < dim; m++)	nextGp[group[i]][m] += data[i][m];
		}

		double error = 0;
		for (int j = 0; j < k; j++) {
			if (sum[j] != 0) { for (int m = 0; m < dim; m++)	nextGp[j][m] /= sum[j]; }
			else { for (int m = 0; m < dim; m++)	nextGp[j][m] = 0; }
			error += GetDistance(nextGp[j], gp[j], dim);
		}
		if (error == 0)break;
		for (int i = 0; i < k; i++) {
			printf("next %lf  ", nextGp[i][0]);
			for (int m = 0; m < dim; m++)gp[i][m] = nextGp[i][m];
		}
		printf("\n");
	}

	char fileName[100];
	sprintf_s(fileName, "kmeans_%s.csv", outputName);
	FILE *fp;
	fopen_s(&fp, fileName, "w");
	for (int i = 0; i < k; i++) {
		for (int m = 0; m < dim; m++)fprintf(fp, "%lf,", gp[i][m]);
		fprintf(fp, "\n");
	}
	fclose(fp);

	//����
	int *number = new(int[k]);
	for (int i = 0; i < k; i++)number[i] = 0;

	for (int i = 0; i < dataNum; i++) {
		sprintf_s(name, "%s\\%d.png", fName, i);
		Mat img = imread(name, 1);
		if (img.data != NULL) {
			string foldName = outputName + to_string(group[i]);
			Directory::MakeDirectory(foldName.c_str());
			foldName += "\\" + to_string(number[group[i]]) + ".png";
		//	sprintf_s(name, "%s%d\\%d.png", outputName, group[i], number[group[i]]);
			imwrite(foldName.c_str(), img);
			number[group[i]]++;
		}
	}

	//���
	for (int i = 0; i < k; i++)delete[] gp[i];
	for (int i = 0; i < k; i++)delete[] nextGp[i];
	for (int i = 0; i < dataNum; i++)delete[] data[i];
	delete[] data;
	delete[] gp;
	delete[] nextGp;
	delete[] group;
	delete[] number;
}























