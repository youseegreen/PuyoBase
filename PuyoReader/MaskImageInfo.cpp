#include "MaskImageInfo.h"

#pragma warning(disable : 4996)
#pragma warning(disable : 4244)


/*�ǂݍ��񂾓���̃X�^�[�g�ʒu�ƃ}�X�N�摜����肷��*/
bool MaskImageInfo::GetMaskInfo(VideoCapture *video, Mat *nowFrame) {
	int lastFrameNum = (int)video->get(CV_CAP_PROP_FRAME_COUNT);
//	printf("%d\n", lastFrameNum);
//	FowardVideoFrame(video, nowFrame, 9500);


	//�͂��߂Ɏ�����ʂ�T��(�X�^�[�g�܂Ń��[�v)
	do {
		*video >> *nowFrame;
		if (video->get(CV_CAP_PROP_POS_FRAMES) > lastFrameNum - 30) {
			cout << "�X�^�[�g�ӏ���������܂���ł���" << endl;
			return false;
		}
	} while (!IsStartFrame(nowFrame));		//�}�X�N�摜����肷�邽��IsStart��mask�w��

	printf("%d�t���[����\n", (int)video->get(CV_CAP_PROP_POS_FRAMES));
	FowardVideoFrame(video, nowFrame, 10);	//�ύX�̗]�n����
	//���̉�ʂ���}�X�N�摜����肷��
	CaluculateMaskImage(nowFrame);
	//ForDebug("fordebug");	
	return true;
}









/*�n�t�ϊ����g����1�����̃X�^�[�g��T���o��*/
bool MaskImageInfo::IsStartFrame(Mat *nowFrame) {
	static int blackFrag = 0;
	if (IsBlackImage(nowFrame)) {
//		imshow("tmp", *nowFrame);
//		waitKey(1);
		blackFrag = 100;	//100�𒼂�
		return false;	//���h��摜�Ȃ�X�L�b�v	���Ȃ��Č�100�t���[������Ƃ�
	}
	if(--blackFrag < 1)return false ;	//�O100�t���[����������Ȃ�������return
	

	int wid = (*nowFrame).cols;
	int hei = (*nowFrame).rows;

	for (int i = 0; i < 2; i++) {	//�v���C���[���Ƃɔ���
		Mat playerImage(*nowFrame, Rect(i*wid / 2, 0, wid / 2, hei));	//�v���C���[���̉摜�����

		//������
		GaussianBlur(playerImage, playerImage, Size(7, 7), 0);
		GaussianBlur(playerImage, playerImage, Size(7, 7), 0);
		Mat channels[3];
		split(playerImage, channels);
		Mat canny_r, canny_g, canny_b, canny_image;
		Canny(channels[2], canny_r, 40.0, 180.0, 3);
		Canny(channels[1], canny_g, 40.0, 180.0, 3);
		Canny(channels[0], canny_b, 40.0, 180.0, 3);
		bitwise_or(canny_r, canny_b, canny_image);
		bitwise_or(canny_image, canny_b, canny_image);

		vector<Vec2f> fieldBorders;
		HoughLines(canny_image, fieldBorders, 1, CV_PI / 180, 60);
		Mat edgeImage(playerImage.size(), CV_8UC3, Scalar::all(0));	//���h��摜
		float rho, theta, cost, sint;	//��
		int z = canny_image.cols;
		for (auto it = fieldBorders.begin(); it != fieldBorders.end(); ++it) {
			rho = (*it)[0];
			theta = (*it)[1];
			if (((theta > -0.01) && (theta < 0.01)) ||	//���������T��
				((theta > 1.56) && (theta < 1.58))) {	//90�x
				cost = cos(theta);
				sint = sin(theta);
				//
				line(edgeImage, Point(rho*cost - z * sint, rho*sint + z * cost),
					Point(rho*cost + z * sint, rho*sint - z * cost),
					Scalar(255, 255, 255));
			}
		}
		
//		imshow("edge", canny_image);
//		imshow("origin", playerImage);
//		waitKey(10);


		//�R�[�i�[���o
		Point gravity;
		gravity.x = edgeImage.cols / 2;
		gravity.y = edgeImage.rows / 2;
		FieldBorders border;
		if (!RThita(&edgeImage, gravity, &border, edgeImage.cols, edgeImage.rows))return false;


		//���肵���t�B�[���h�G���A���Ó��Ȃ��̂����f����
		int fieldArea = (border.down - border.up)*(border.right - border.left);
		int ref = wid * hei / 2;
		if ((fieldArea > 0.5 * ref) || (fieldArea < 0.2*ref))return false;

		//�����܂ŗ������_�ŁA�X�^�[�g�G���A�@�Z�b�g���Ă���
		if (i == 0) { p1.SetMaskCorner(border); }
		if (i == 1) { p2.SetMaskCorner(border); }
	
	}
	return true;
}





const void MaskImageInfoOnePlayer::GetSrcDifferentImage(Mat *output, const Mat *input) {
	*output = input->clone();
	//���͉摜�ƌ��摜�̌덷��+-1�ȉ��Ȃ獕�̉摜��Ԃ�
	for (int y = 0; y < output->rows; y++) {
		for (int x = 0; x < output->cols; x++) {
			int frag = 0;
			for (int c = 0; c < output->channels(); c++) {
				int t = (int)(y * src.step + x * src.channels() + c);
				if (abs(output->data[t] - src.data[t]) < 20)frag++;
			}
			if (frag == 3) {
				for (int c = 0; c < output->channels(); c++) {
					output->data[y*src.step + x * src.channels() + c] = 0;
				}
			}
		}
	}
}

void const MaskImageInfo::GetSrcDifferentImage(cv::Mat *output, int player, const cv::Mat *input) {
	if (player == 0) {
		Rect rect(0, 0, input->cols / 2, input->rows);
		Mat in(*input, rect);
		p1.GetSrcDifferentImage(output, &in);
	}
	else {
		Rect rect(input->cols / 2, 0, input->cols / 2, input->rows);
		Mat in(*input, rect);
		p2.GetSrcDifferentImage(output, &in);
	}
}

void MaskImageInfoOnePlayer::GetMaskImageField(Mat *maskImg, int x, int y, const Mat *ref) {
	Rect rec(fieldBorders.left + x * width, fieldBorders.up + y * height, width, height);
	if (ref == NULL) { Mat tmp(src, rec); *maskImg = tmp.clone(); }
	else { Mat tmp(*ref, rec); *maskImg = tmp.clone(); }
}

//���_
void MaskImageInfoOnePlayer::GetMaskImagePoint(cv::Mat *maskImg, const cv::Mat *ref) {
	Rect rec(fieldBorders.left, fieldBorders.up + 11 * height, width * 2, height * 2);
	Mat tmp(*ref, rec);
	*maskImg = tmp.clone();
}

//0,0  0,1  1,0  1,1  �ȊO�͑S���o�͂���悤�ɐݒ�
void MaskImageInfoOnePlayer::GetMaskImageCircle(Mat *maskImg, int next, int num, const Mat *ref) {
	int c = next * 2 + num;

	//�S�͈�
	if (c > 3) {
		Size imgSize;
		if (ref == NULL) imgSize = src.size();
		else imgSize = ref->size();
		Mat zero(src.size(), CV_8UC3, Scalar::all(0));
		circle(zero, nex1.center, nex1.r, Scalar(255, 255, 255), -1);
		circle(zero, nex2.center, nex2.r, Scalar(255, 255, 255), -1);
		circle(zero, nexnex1.center, nexnex1.r, Scalar(255, 255, 255), -1);
		circle(zero, nexnex2.center, nexnex2.r, Scalar(255, 255, 255), -1);
		if (ref == NULL) bitwise_and(zero, src, zero);
		else bitwise_and(zero, *ref, zero);
		*maskImg = zero.clone();
		return;
	}

	//����ȊO
	Rect tmp;
	Size imgSize;
	if (ref == NULL) imgSize = src.size();
	else imgSize = ref->size();
	Mat zero(src.size(), CV_8UC3, Scalar::all(0));
	switch (c) {
	case 0:
		circle(zero, nex1.center, nex1.r, Scalar(255, 255, 255), -1);
		tmp.width = tmp.height = nex1.r * 2;
		tmp.x = nex1.center.x - nex1.r;
		tmp.y = nex1.center.y - nex1.r;
		break;
	case 1:
		circle(zero, nex2.center, nex2.r, Scalar(255, 255, 255), -1);
		tmp.width = tmp.height = nex2.r * 2;
		tmp.x = nex2.center.x - nex2.r;
		tmp.y = nex2.center.y - nex2.r;
		break;
	case 2:
		circle(zero, nexnex1.center, nexnex1.r, Scalar(255, 255, 255), -1);
		tmp.width = tmp.height = nexnex1.r * 2;
		tmp.x = nexnex1.center.x - nexnex1.r;
		tmp.y = nexnex1.center.y - nexnex1.r;
		break;
	case 3:
		circle(zero, nexnex2.center, nexnex2.r, Scalar(255, 255, 255), -1);
		tmp.width = tmp.height = nexnex2.r * 2;
		tmp.x = nexnex2.center.x - nexnex2.r;
		tmp.y = nexnex2.center.y - nexnex2.r;
		break;
	}
	if (ref == NULL) bitwise_and(zero, src, zero);
	else bitwise_and(zero, *ref, zero);
	Mat tmp2(zero, tmp);
	*maskImg = tmp2.clone();
}




bool MaskImageInfoOnePlayer::IsGameFinished(Mat *nowFrame) {
	Rect rect = deathJudge;
	if (player == 1) rect.x += (nowFrame->cols / 2);
	Mat tmp(*nowFrame, rect);
	double val[3] = { 0 };
	for (int y = 0; y < tmp.rows; y++) {
		for (int x = 0; x < tmp.cols; x++) {
			for (int c = 0; c < tmp.channels(); c++) {
				val[c] += ((double)tmp.data[y * tmp.step + x * tmp.channels() + c] / tmp.rows / tmp.cols);
			}
		}
	}
	double error = 0;
	for (int i = 0; i < 3; i++)error += abs(val[i] - deathValue[i]);
	//	printf("%lf  ", error);
	if (error > 50)return true;
	return false;
}



void MaskImageInfoOnePlayer::CaluculateMaskImage(Mat *input) {
	width = double((fieldBorders.right - fieldBorders.left)) / 6.0;
	height = double((fieldBorders.down - fieldBorders.up)) / 12.0;

	//�v���l
	nexnex1.r = 0.36 * width;
	nexnex2.r = 0.36 * width;
	nex1.r = 0.48 * width;
	nex2.r = 0.48 * width;

	//1p�Ȃ�
	if (player == 0) {
		Mat tmp(*input, Rect(0, 0, input->cols / 2, input->rows));
		src = tmp.clone();
		nex1.center.x = fieldBorders.right + width * 6 * 0.215;
		nex2.center.x = fieldBorders.right + width * 6 * 0.215;
		nexnex1.center.x = fieldBorders.right + width * 6 * 0.335;
		nexnex2.center.x = fieldBorders.right + width * 6 * 0.335;
		Rect rect2(fieldBorders.left - width, fieldBorders.up + height, width, fieldBorders.down - fieldBorders.up - 2 * height);
		deathJudge = rect2;
	}
	//2p�Ȃ�
	else {
		//�ȒP��������
		Mat tmp(*input, Rect(input->cols / 2, 0, input->cols / 2, input->rows));
		src = tmp.clone();//
		nex1.center.x = fieldBorders.left - width * 6 * 0.215 - 1;
		nex2.center.x = fieldBorders.left - width * 6 * 0.215 - 1;
		nexnex1.center.x = fieldBorders.left - width * 6 * 0.335 - 1;
		nexnex2.center.x = fieldBorders.left - width * 6 * 0.335 - 1;
		//�ȒP��������
		Rect rect2(fieldBorders.right, fieldBorders.up + height, width, fieldBorders.down - fieldBorders.up - 2 * height);
		deathJudge = rect2;
	}
	Mat tmp2(src, deathJudge);
	for (int y = 0; y < tmp2.rows; y++) {
		for (int x = 0; x < tmp2.cols; x++) {
			for (int c = 0; c < tmp2.channels(); c++) {
				deathValue[c] += (double)tmp2.data[y * tmp2.step + x * tmp2.channels() + c] / tmp2.cols / tmp2.rows;
			}
		}
	}

	nex1.center.y = fieldBorders.up + width * 6 * 0.15;
	nex2.center.y = fieldBorders.up + width * 6 * 0.305;
	nexnex1.center.y = fieldBorders.up + width * 6 * 0.43;
	nexnex2.center.y = fieldBorders.up + width * 6 * 0.545;
}



void MaskImageInfo::ForDebug(const char *fName) {
	char buf1[200];
	sprintf(buf1, "%s\\p1_", fName);
	p1.ForDebug(buf1);
	char buf2[200];
	sprintf(buf2, "%s\\p2_", fName);
	p2.ForDebug(buf2);
}

void MaskImageInfoOnePlayer::ForDebug(char *fName) {
	Mat tmp1 = src.clone();
	cv::line(tmp1, Point(fieldBorders.left, fieldBorders.up), Point(fieldBorders.right, fieldBorders.up), Scalar(255, 0, 0));
	cv::line(tmp1, Point(fieldBorders.left, fieldBorders.down), Point(fieldBorders.right, fieldBorders.down), Scalar(255, 0, 0));
	cv::line(tmp1, Point(fieldBorders.left, fieldBorders.up), Point(fieldBorders.left, fieldBorders.down), Scalar(255, 0, 0));
	cv::line(tmp1, Point(fieldBorders.right, fieldBorders.up), Point(fieldBorders.right, fieldBorders.down), Scalar(255, 0, 0));
	char name1[200];
	sprintf(name1, "%sfield.png", fName);
	imwrite(name1, tmp1);

	for (int y = 0; y < 12; y++) {
		for (int x = 0; x < 6; x++) {
			Rect rec(fieldBorders.left + x * width, fieldBorders.up + y * height, width, height);
			Mat tmp(src, rec);
			char name[200];
			sprintf(name, "%sy=%d_x=%d.png", fName, y, x);
			imwrite(name, tmp);
		}
	}

	Mat zero(src.size(), CV_8UC3, Scalar::all(0));
	Mat t = zero.clone();
	circle(t, nex1.center, nex1.r, Scalar(255, 255, 255), -1);
	bitwise_and(t, src, t);
	sprintf(name1, "%snex1.png", fName);
	imwrite(name1, t);
	t = zero.clone();
	circle(t, nex2.center, nex2.r, Scalar(255, 255, 255), -1);
	bitwise_and(t, src, t);
	sprintf(name1, "%snex2.png", fName);
	imwrite(name1, t);
	t = zero.clone();
	circle(t, nexnex1.center, nexnex1.r, Scalar(255, 255, 255), -1);
	bitwise_and(t, src, t);
	sprintf(name1, "%snexnex1.png", fName);
	imwrite(name1, t);
	t = zero.clone();
	circle(t, nexnex2.center, nexnex2.r, Scalar(255, 255, 255), -1);
	bitwise_and(t, src, t);
	sprintf(name1, "%snexnex2.png", fName);
	imwrite(name1, t);

	Mat tmp(src, deathJudge);
	sprintf(name1, "%sdeath.png", fName);
	imwrite(name1, tmp);
}

void MaskImageInfo::GetMaskImageField(cv::Mat *maskImg, int player, int x, int y, const Mat *ref) {
	if (player == 0) {
		/*	if (ref == NULL)p1.GetMaskImageField(maskImg, x, y);
			else {
				Rect rect(0, 0, ref->cols / 2, ref->rows);
				Mat in(*ref, rect);
				p1.GetMaskImageField(maskImg, x, y, &in);
			}*/
		p1.GetMaskImageField(maskImg, x, y, ref);
	}
	else {
		/*		if (ref != NULL)p2.GetMaskImageField(maskImg, x, y);
				else {
					Rect rect(ref->cols / 2, 0, ref->cols / 2, ref->rows);
					Mat in(*ref, rect);
					p2.GetMaskImageField(maskImg, x, y, &in);
				}*/
		p2.GetMaskImageField(maskImg, x, y, ref);
	}
}

void MaskImageInfo::GetMaskImageCircle(cv::Mat *maskImg, int player, int next, int num, const Mat *ref) {
	if (player == 0) {
		if (ref == NULL)p1.GetMaskImageCircle(maskImg, next, num);
		else {
			Rect rect(0, 0, ref->cols / 2, ref->rows);
			Mat in(*ref, rect);
			p1.GetMaskImageCircle(maskImg, next, num, &in);
		}
	}
	else {
		if (ref == NULL)p2.GetMaskImageCircle(maskImg, next, num);
		else {
			Rect rect(ref->cols / 2, 0, ref->cols / 2, ref->rows);
			Mat in(*ref, rect);
			p2.GetMaskImageCircle(maskImg, next, num, &in);
		}
	}
}

void MaskImageInfo::GetMaskImagePoint(cv::Mat *maskImg, int player, const cv::Mat *ref) {
	if (player == 0) {
		Rect rect(0, 0, ref->cols / 2, ref->rows);
		Mat in(*ref, rect);
		p1.GetMaskImagePoint(maskImg, &in);
	}
	else {
		Rect rect(ref->cols / 2, 0, ref->cols / 2, ref->rows);
		Mat in(*ref, rect);
		p2.GetMaskImagePoint(maskImg, &in);
	}
}




/*���͎g���ĂȂ�
void SearchGameState(VideoCapture *input, int wid, int hei, int *start) {
Mat frame;
do {
*input >> frame;
imwrite("test.png", frame);	//
} while (!IsStart(&frame, wid, hei));
FowardCap(input, &frame,10);
imwrite("correct.png", frame);
}*/