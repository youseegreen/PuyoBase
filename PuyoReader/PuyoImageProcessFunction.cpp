#include "PuyoImageProcessFunction.h"

bool IsMovingFrame(Mat *input, Mat *ref, MaskImageInfo *mask);
//Color PuyoDiscriminate(double mean[3], double dis[3]);
Color PuyoDiscriminate(Mat &img);

void SearchBeforeOjamaDropFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum);
bool IsChain(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum);

bool isCollectData = false;

//�f�[�^�쐬�p
void SaveOnePuyoImage2(const Mat *img, const Color col) {
	static int NCount = 0;
	static int puyoImgNum = (int)(Directory::GetFileNameList("kuro\\")).size();
	if (col == N) { NCount = (NCount + 1) % 1000; }	//N��100��Ɉ�񂾂����
	if (col == N && NCount != 0) { printf("a"); return; }
//	if (col == R || col == G || col == B || col == Y || col == P || col == O) {
		string puyoImgName = string("kuro\\") + to_string(puyoImgNum) + ".png";
		imwrite(puyoImgName.c_str(), *img);
		puyoImgNum++;
//	}
}

void SaveOnePuyoImage(const Mat *img, const Color col) {
//	static int NCount = 0;
	static int colorCount[8] = { 0 };
//	static int puyoImgNum = (int)(Directory::GetFileNameList("kuro\\")).size();
//	if (col == N) { NCount = (NCount + 1) % 1000; }	//N��100��Ɉ�񂾂����
//	if (col == N && NCount != 0) { printf("a"); return; }
	//	if (col == R || col == G || col == B || col == Y || col == P || col == O) {
	string puyoImgName = string("kuro_") + to_string(col) + string("\\") + to_string(colorCount[(int)col])+".png";
	imwrite(puyoImgName.c_str(), *img);
	colorCount[(int)col]++;
	//	}
}


//1��mp4�t�@�C�����珇�ɂՂ效�𓾂�
void GetPUYOHUFromOneVideo(const char *fName) {
	//�ۑ��t�H���_�쐬
	string videoFileName(fName);
	//�t���p�X\hoge.mp4��hoge�����ɂ���
	string fileNameOnly = videoFileName.substr(videoFileName.find_last_of("\\") + 1,
		videoFileName.find_last_of(".") - videoFileName.find_last_of("\\") - 1);
	Directory::MakeDirectory(fileNameOnly.c_str());


	//����̓ǂݍ��݋y�ъ�{���̓���
	VideoCapture video(fName);
	GetVideoInfo(&video);	//��{���̕\��

	//�X�^�[�g�ʒu��T��&���̓���̃}�X�N�摜�����
	MaskImageInfo mask;
	Mat nowFrame;	//���ݒ��ڂ��Ă���t���[��
	DataInputor data[2];	//now, prev
	int gameNum = 0;

	if (!mask.GetMaskInfo(&video, &nowFrame)) { printf("�_���ł���\n"); return; };	//���̎��_�ŃX�^�[�g�ɂƂ�ł���

	/*�������烋�[�v*/
	do {
		printf("%d������\n", gameNum);

		//�����Ŏ������̃t�H���_������悤�ɂ���
		Directory::MakeDirectory((fileNameOnly + string("\\") + to_string(gameNum)).c_str());


		int moveTimes = 0;	//n-1��ڂ�����
		const int startFrameCount = (int)video.get(CV_CAP_PROP_POS_FRAMES);
		const double fps = (double)video.get(CV_CAP_PROP_FPS);
	//	printf("%d, %lf\n", startFrameCount, fps);
		//���̉������Ă���

		//�ǂ��炩�����ʂ܂�
		while (mask.IsGameFinished(&nowFrame) == -1) {	//����
			try {
				//���ݎ���L�^����
				DiscriminateNow(&nowFrame, &mask, &data[moveTimes % 2]);
				//�����ň�񌻍݌��Ă�t���[���ԍ����L�^����
				int prevFrameNum = (int)video.get(CV_CAP_PROP_POS_FRAMES);
				//1��i�񂾏u�Ԃ�������
				SearchMoving(&video, &mask, &nowFrame);
				//���̎��̃t�B�[���h��F������
				DiscriminateField(&nowFrame, &mask, &data[moveTimes % 2]);
				DiscriminateNextField(&nowFrame, &mask, &data[(moveTimes + 1) % 2]);
				//�O�̃t�B�[���h�ƌ��݂̃t�B�[���h�ł����������������`�F�b�N����Ȃ��Ȃ�A
				if (data[(moveTimes + 1) % 2].IsChainOrOjama()) {	//firstFlag?�@�@//����
					//�܂����̃t���[���ԍ����L�^���Ă���
					int nowFrameNum = (int)video.get(CV_CAP_PROP_POS_FRAMES);
					//�Ղ����or������܂ӂ�O��T��  ���߂̃t���[���ԍ��ɖ߂��� 
					//�Ղ������������ܗ���  ���̃t���[���ԍ��ɖ߂��Ă���
					SearchPuyoJustSetted(&video, &mask, &nowFrame, prevFrameNum, nowFrameNum);	//����
					//�Q�b�g����nowFrame�摜����l�N�X�g�t�B�[���h�Ċm�F
					DiscriminateNextField(&nowFrame, &mask, &data[(moveTimes + 1) % 2]);
				}

				//�����Ő����f�[�^���Q�b�g����
				data[(moveTimes + 1) % 2].GetCorrect();	//����
				//5�F����4�F�ɂ���
			//	data[(moveTimes + 1) % 2].Color5To4();	//����
				data[moveTimes % 2].SetTime(prevFrameNum, startFrameCount, fps);

				//�ۑ�����
				if (moveTimes > 0)	data[(moveTimes + 1) % 2].WriteFile(
					(fileNameOnly + string("\\") + to_string(gameNum) + string("\\") + to_string(moveTimes) + string(".csv")).c_str()
					//(fileNameOnly + string("\\") + to_string(gameNum) + string("\\") + to_string(1p or 2p) + string("\\")
					// + to_string(moveTimes) + string(".csv")).c_str()
				);
				//�l�N�X�g�̓������~�܂�܂�cap�������߂�
				FowardFrameUntillDontMove(&video, &mask, &nowFrame);	//����
				//�l�N�X�g�ƃl�N�l�N���L�^����
				DiscriminateNext(&nowFrame, &mask, &data[moveTimes % 2]);

				moveTimes++;
			}
			catch (cv::Exception) {
				printf("error\n");
				moveTimes--;
				return;
			}
		}
		gameNum++;
		//���̃Q�[���̃X�^�[�g�܂Ŕ�ԁ��n�t�ϊ����ł���10�t���[���Ƃ΂�
	} while (SkipToNextGameStartFrame(&video, &mask, &nowFrame));	//����
}



///*�肪�ς��u�Ԃ̃t���[����T��*/
//void SearchChangingFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame) {
//	Mat ref = movingFrame->clone();		//��摜�����(movingFrame�ɓ����Ă�K�v������)
//	do {
//		*input >> *movingFrame;
//	} while (!IsMovingFrame(movingFrame, &ref, mask));	//�}�X�N�摜����肷�邽��IsStart��mask�w��
//}






/*�肪�ς��u�Ԃ̃t���[����T��*/
void SearchMoving(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame) {
	Mat ref = movingFrame->clone();		//��摜�����(movingFrame�ɓ����Ă�K�v������)
	do {
		*input >> *movingFrame;
	} while (!IsMovingFrame(movingFrame, &ref, mask));	//�}�X�N�摜����肷�邽��IsStart��mask�w��
}

/*��摜�ƃ}�X�N�摜���玟�̎�ɂȂ������𔻒f����*/
bool IsMovingFrame(Mat *input, Mat *ref, MaskImageInfo *mask) {
	Mat img1, img2;
	bool flag = true;
	//1p�̃l�N�X�g1,2�A�l�N�l�N1,2����ƌ��݃t���[���ł���@�������Ă����S�ĕς�����Ƃ��̂�OK�Ƃ���
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			mask->GetMaskImageCircle(&img1, 0, i, j, input);	//�����
			mask->GetMaskImageCircle(&img2, 0, i, j, ref);
			//img1��img2�̊e�s�N�Z�����Ƃ̌덷���L�^����
			int error = Caluculate2ImageError(&img1, &img2);
			if (error < EOL) flag = false;
		}
	}
	if (flag)return true;
	return false;
}


bool SkipToNextGameStartFrame(cv::VideoCapture *video, MaskImageInfo *mask, cv::Mat *nowFrame) {
	int lastFrameNum = (int)video->get(CV_CAP_PROP_FRAME_COUNT);
	//�͂��߂Ɏ�����ʂ�T��(�X�^�[�g�܂Ń��[�v)
	do {
		*video >> *nowFrame;
		if (video->get(CV_CAP_PROP_POS_FRAMES) > lastFrameNum - 30) {
			cout << "�X�^�[�g�ӏ���������܂���ł���" << endl;
			return false;
		}
	} while (!mask->IsStartFrame(nowFrame));		//�}�X�N�摜����肷�邽��IsStart��mask�w��

	FowardVideoFrame(video, nowFrame, 10);	//�ύX�̗]�n����
	return true;
}


//�Ղ����or������܂ӂ�O��T��	���߂̃t���[���ԍ��ɖ߂��ā@�@�Ղ������������ܗ����@�@�Ō�ɍ��̃t���[���ԍ��ɖ߂��Ă���
void SearchPuyoJustSetted(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame, int startNum, int endNum) {
	//�X�^�[�g�t���[���ɖ߂���movingFrame�����̃t���[���ɖ߂�
	input->set(CV_CAP_PROP_POS_FRAMES, startNum);
	*input >> *movingFrame;

	//�A�����N�������`�F�b�N
	if (IsChain(input, mask, movingFrame, startNum, endNum)) {
		//�A���N���Ă�Ȃ�cap���ŏ��̃t���[���ɖ߂���return
		input->set(CV_CAP_PROP_POS_FRAMES, endNum);
		return;
	}

	//�A�����N���Ă��Ȃ��Ȃ炨�ז���������܂Ń`�F�b�N
	//�X�^�[�g�t���[���ɖ߂���movingFrame�����̃t���[���ɖ߂�
	input->set(CV_CAP_PROP_POS_FRAMES, startNum);
	*input >> *movingFrame;

	SearchBeforeOjamaDropFrame(input, mask, movingFrame, startNum, endNum);
	//cap���ŏ��̃t���[���ɖ߂���return
	input->set(CV_CAP_PROP_POS_FRAMES, endNum);
}


//�l�N�X�g�̓������~�܂�܂�cap�������߂�
void FowardFrameUntillDontMove(cv::VideoCapture *input, MaskImageInfo *mask, cv::Mat *nowFrame) {
	const int A = 4;
	Mat prev[A], img[A + 1];
	int error = 0;
	for (int i = 0; i < A - 1; i++) {
		prev[i] = nowFrame->clone();
		FowardVideoFrame(input, nowFrame, 1);
	}

	do {
		error = 0;
		for (int i = A - 1; i > 0; i--) {
			prev[i] = prev[i - 1].clone();
		}
		prev[0] = nowFrame->clone();
		*input >> *nowFrame;
		mask->GetMaskImageCircle(&img[0], 0, 2, 2, nowFrame);	//�S�͈�
		for (int i = 1; i < A + 1; i++) {
			mask->GetMaskImageCircle(&img[i], 0, 2, 2, &prev[i - 1]);
		}
		for (int i = 1; i < A + 1; i++) {
			error += Caluculate2ImageError(&img[0], &img[i]);
		}
		//		printf("%d \n", error);
	} while (error > 0);
	input->set(CV_CAP_PROP_POS_FRAMES, input->get(CV_CAP_PROP_POS_FRAMES) - A + 1);
}






void DiscriminateNow(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	Mat img;
	for (int j = 0; j < 2; j++) {
		mask->GetMaskImageCircle(&img, 0, 0, j, frame);
	//	double mean[3] = { 0 }, dis[3] = { 0 };	//���ςƕ��U
	//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
	//	Color col = PuyoDiscriminate(mean, dis);
		Color col = PuyoDiscriminate(img);
		data->InputNow(j, col);

		if (isCollectData) SaveOnePuyoImage(&img, col);
	}
}

void DiscriminateNext(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	Mat img;
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			mask->GetMaskImageCircle(&img, 0, i, j, frame);
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//���ςƕ��U
		//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
		//	Color col = PuyoDiscriminate(mean, dis);
			Color col = PuyoDiscriminate(img);

			data->InputNext(i, j, col);
			if (isCollectData) SaveOnePuyoImage(&img, col);
		}
	}
}

//fix?�@���n�����āA���o�X�g�ɂ���H
void DiscriminateField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	//�܂���1p�̃t�B�[���h����  �l�N�X�g�̂ق�(Next)�͕ω����Ă�Ƃ��g�����炠���񂱂Ƃɒ���
	Mat img, diffImage;
	mask->GetSrcDifferentImage(&diffImage, 0, frame);
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 12; y++) {
			((MaskImageInfo)(*mask)).GetMaskImageField(&img, 0, x, y, &diffImage);	//���݃t���[���̉摜
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//���ςƕ��U
		//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
		//	Color col = PuyoDiscriminate(mean, dis);
			Color col = PuyoDiscriminate(img);
			data->InputField(x, y, col);

			if(isCollectData) SaveOnePuyoImage(&img,col);
		}
	}
}

void DiscriminateNextField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	//�܂���1p�̃t�B�[���h����  �l�N�X�g�̂ق�(Next)�͕ω����Ă�Ƃ��g�����炠���񂱂Ƃɒ���
	Mat img, diffImage;
	mask->GetSrcDifferentImage(&diffImage, 0, frame);
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 12; y++) {
			((MaskImageInfo)(*mask)).GetMaskImageField(&img, 0, x, y, &diffImage);	//���݃t���[���̉摜
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//���ςƕ��U
		//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
		//	Color col = PuyoDiscriminate(mean, dis);
			Color col = PuyoDiscriminate(img);
			data->InputNextField(x, y, col);

		}
	}
}






Color PuyoDiscriminate(Mat &img) {
	double vote[8] = { 0 };
	Mat hsv;
	cvtColor(img, hsv, COLOR_BGR2HSV);

	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			int r = img.data[y*img.step + x * img.channels() + 2];
			int g = img.data[y*img.step + x * img.channels() + 1];
			int b = img.data[y*img.step + x * img.channels() + 0];
			int h = img.data[y*hsv.step + x * hsv.channels() + 0];
			int s = img.data[y*hsv.step + x * hsv.channels() + 1];
			int v = img.data[y*hsv.step + x * hsv.channels() + 2];

			if (r < 100 && g < 100 && b < 100) {
				vote[0]--;	//��
			}
	//		else if (g > 120 && g > r + 30 && r > b + 5 && h > 60 && h < 100) {
			else if (g > 120 && g > r + 30 && r > b + 5) {
				vote[2]--;	//��
			}
	//		else if (b > g + 40 && g > r + 40 && h > 120 && h < 150) {
			else if (b > g + 40 && g > r + 40) {
				vote[3]--;	//��
			}
	//		else if (abs(b - r) < 45 && r > g + 40 && h > 170 && h < 200) {
			else if (abs(b - r) < 45 && r > g + 40) {
				vote[5]--;	//��
			}
	//		else if (r > 200 && r > g + 40 && abs(g - b) < 40 && (h > 220 || h < 10)) {
			else if (r > 200 && r > g + 40 && abs(g - b) < 40) {
				vote[1]--;	//��
			}
	//		else if (r > 200 && g > b + 40 && h > 20 && h < 40 ) {
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
	if (-vote[0] > img.rows*img.cols*0.5) vote[0] = -img.rows*img.cols;//��
	else vote[0] = 0;

	double minValue; int min;
	SearchMinimum(vote, 8, minValue, min);
	if (min == 7) {
		vote[7] = 0;
		SearchMinimum(vote, 8, minValue, min);
		if (minValue > -6) { 
			min = 7; 
		//	imshow("hoge", img);
		//	waitKey(0);
		}
	}
	if (min == 0)return N;
	if (min == 1)return R;
	if (min == 2)return G;
	if (min == 3)return B;
	if (min == 4)return Y;
	if (min == 5)return P;
	if (min == 6)return O;
	if (min == 7)return N;
//�� R231  G=B   R>G=B
//�� R29-100   G88-150      B215-233        R>G>B
//�� R255  G130-255  B80-128  R=255 G>B
//�� G>R>B
//�� R=B>G   B>R>>G
//���ז� R=G=B
}






////6�����x�N�g���Ŕ��ʂ���
//Color PuyoDiscriminate(double mean[3], double dis[3]) {
//	double input[6];
//	for (int j = 0; j < 3; j++)input[j] = mean[j];
//	for (int j = 0; j < 3; j++)input[j + 3] = dis[j];
//
//	//�d�S���Q�b�g����
//	//�d�S�����z��
//	double firstGp[5][6];
//	double secondGp[3][2][6];
//	char fName[100];
//	FILE *fp;
//	fopen_s(&fp, "JudgeColor\\first.csv", "r");
//	for (int i = 0; i < 5; i++) {
//		for (int j = 0; j < 6; j++) {
//			fscanf_s(fp, "%lf,", &(firstGp[i][j]));
//		}
//		fscanf_s(fp, "\n");
//	}
//	fclose(fp);
//	for (int l = 0; l < 3; l++) {
//		sprintf_s(fName, "JudgeColor\\%d.csv", l);
//		fopen_s(&fp, fName, "r");
//		for (int i = 0; i < 2; i++) {
//			for (int j = 0; j < 6; j++) {
//				fscanf_s(fp, "%lf,", &(secondGp[l][i][j]));
//			}
//			fscanf_s(fp, "\n");
//		}
//		fclose(fp);
//	}


//	//�����ŒZ��T��
//	double firstDistance[5];
//	double secondDistance[2];
//	int min2; double minValue2;
//
//	for (int j = 0; j < 5; j++) firstDistance[j] = GetDistance(input, firstGp[j], 6);
//	int min; double minValue;
//	SearchMinimum(firstDistance, 5, minValue, min);
//
//	switch (min) {
//	case 0:
//	case 4:
//		//NO
//		return N;
//	case 1:
//		//RG���킯��
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[0][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return P;
//		else return R;
//	case 2:
//		//GB���킯��
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[1][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return G;
//		else return B;
//	case 3:
//		//YO���킯��
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[2][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return Y;
//		else return O;
//	}
//	return N;
//}




























//�A���N���Ă��邩�`�F�b�N  ���_�������Ă��邩�Ŕ���
bool IsChain(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum) {
	int c = 0, th = 10; const int A = 1;
	Mat prev[A], img[A + 1];
	for (int i = 0; i < A - 1; i++) {
		prev[i] = nowFrame->clone();
		FowardVideoFrame(input, nowFrame, 1);
	}

	//�����ɒT������̂�h������
	while (endNum - startNum + 1 > c) {
		c++;
		int error = 0;

		for (int i = A - 1; i > 0; i--) {
			prev[i] = prev[i - 1].clone();
		}
		prev[0] = nowFrame->clone();
		*input >> *nowFrame;

		mask->GetMaskImagePoint(&img[0], 0, nowFrame);	//���_�̍����݂�
		for (int i = 1; i < A + 1; i++) {
			mask->GetMaskImagePoint(&img[i], 0, &prev[i - 1]);
		}

		//�덷���v�Z����
		for (int i = 1; i < A + 1; i++) {
			error += Caluculate2ImageError(&img[0], &img[i]);
		}
		//	printf("%d\n", error);

		if (error > th) {
			int n = (int)(input->get(CV_CAP_PROP_POS_FRAMES) - A - 3);		//3�͒�������
			input->set(CV_CAP_PROP_POS_FRAMES, n);
			*input >> *nowFrame;
			return true;
		}
	}
	return false;
}




//���ז����~��O�̃t���[���ɃZ�b�g
void SearchBeforeOjamaDropFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum) {
	//����endNum����H�t���[���O�̃t���[���ɂ��Ƃ���
	input->set(CV_CAP_PROP_POS_FRAMES, endNum - 5);
	*input >> *nowFrame;
}

