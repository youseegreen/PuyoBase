#include "PuyoImageProcessFunction.h"

bool IsMovingFrame(Mat *input, Mat *ref, MaskImageInfo *mask);
//Color PuyoDiscriminate(double mean[3], double dis[3]);
Color PuyoDiscriminate(Mat &img);

void SearchBeforeOjamaDropFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum);
bool IsChain(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum);

bool isCollectData = false;

//データ作成用
void SaveOnePuyoImage2(const Mat *img, const Color col) {
	static int NCount = 0;
	static int puyoImgNum = (int)(Directory::GetFileNameList("kuro\\")).size();
	if (col == N) { NCount = (NCount + 1) % 1000; }	//Nは100回に一回だけ取る
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
//	if (col == N) { NCount = (NCount + 1) % 1000; }	//Nは100回に一回だけ取る
//	if (col == N && NCount != 0) { printf("a"); return; }
	//	if (col == R || col == G || col == B || col == Y || col == P || col == O) {
	string puyoImgName = string("kuro_") + to_string(col) + string("\\") + to_string(colorCount[(int)col])+".png";
	imwrite(puyoImgName.c_str(), *img);
	colorCount[(int)col]++;
	//	}
}


//1つのmp4ファイルから順にぷよ譜を得る
void GetPUYOHUFromOneVideo(const char *fName) {
	//保存フォルダ作成
	string videoFileName(fName);
	//フルパス\hoge.mp4→hogeだけにする
	string fileNameOnly = videoFileName.substr(videoFileName.find_last_of("\\") + 1,
		videoFileName.find_last_of(".") - videoFileName.find_last_of("\\") - 1);
	Directory::MakeDirectory(fileNameOnly.c_str());


	//動画の読み込み及び基本情報の入手
	VideoCapture video(fName);
	GetVideoInfo(&video);	//基本情報の表示

	//スタート位置を探す&その動画のマスク画像を入手
	MaskImageInfo mask;
	Mat nowFrame;	//現在注目しているフレーム
	DataInputor data[2];	//now, prev
	int gameNum = 0;

	if (!mask.GetMaskInfo(&video, &nowFrame)) { printf("ダメでした\n"); return; };	//この時点でスタートにとんでいる

	/*ここからループ*/
	do {
		printf("%d試合目\n", gameNum);

		//ここで試合数のフォルダもつくるようにする
		Directory::MakeDirectory((fileNameOnly + string("\\") + to_string(gameNum)).c_str());


		int moveTimes = 0;	//n-1手目をさす
		const int startFrameCount = (int)video.get(CV_CAP_PROP_POS_FRAMES);
		const double fps = (double)video.get(CV_CAP_PROP_FPS);
	//	printf("%d, %lf\n", startFrameCount, fps);
		//この下を見ていく

		//どちらかが死ぬまで
		while (mask.IsGameFinished(&nowFrame) == -1) {	//実装
			try {
				//現在手を記録する
				DiscriminateNow(&nowFrame, &mask, &data[moveTimes % 2]);
				//ここで一回現在見てるフレーム番号を記録する
				int prevFrameNum = (int)video.get(CV_CAP_PROP_POS_FRAMES);
				//1手進んだ瞬間を見つける
				SearchMoving(&video, &mask, &nowFrame);
				//その時のフィールドを認識する
				DiscriminateField(&nowFrame, &mask, &data[moveTimes % 2]);
				DiscriminateNextField(&nowFrame, &mask, &data[(moveTimes + 1) % 2]);
				//前のフィールドと現在のフィールドでただおいただけかチェックじゃないなら、
				if (data[(moveTimes + 1) % 2].IsChainOrOjama()) {	//firstFlag?　　//実装
					//まず今のフレーム番号を記録しておく
					int nowFrameNum = (int)video.get(CV_CAP_PROP_POS_FRAMES);
					//ぷよ消しorおじゃまふる前を探す  初めのフレーム番号に戻して 
					//ぷよ消し→おじゃま落下  今のフレーム番号に戻しておく
					SearchPuyoJustSetted(&video, &mask, &nowFrame, prevFrameNum, nowFrameNum);	//実装
					//ゲットしたnowFrame画像からネクストフィールド再確認
					DiscriminateNextField(&nowFrame, &mask, &data[(moveTimes + 1) % 2]);
				}

				//ここで正解データをゲットする
				data[(moveTimes + 1) % 2].GetCorrect();	//実装
				//5色から4色にする
			//	data[(moveTimes + 1) % 2].Color5To4();	//実装
				data[moveTimes % 2].SetTime(prevFrameNum, startFrameCount, fps);

				//保存する
				if (moveTimes > 0)	data[(moveTimes + 1) % 2].WriteFile(
					(fileNameOnly + string("\\") + to_string(gameNum) + string("\\") + to_string(moveTimes) + string(".csv")).c_str()
					//(fileNameOnly + string("\\") + to_string(gameNum) + string("\\") + to_string(1p or 2p) + string("\\")
					// + to_string(moveTimes) + string(".csv")).c_str()
				);
				//ネクストの動きが止まるまでcapをすすめる
				FowardFrameUntillDontMove(&video, &mask, &nowFrame);	//実装
				//ネクストとネクネクを記録する
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
		//次のゲームのスタートまで飛ぶ＝ハフ変換ができて10フレームとばす
	} while (SkipToNextGameStartFrame(&video, &mask, &nowFrame));	//実装
}



///*手が変わる瞬間のフレームを探す*/
//void SearchChangingFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame) {
//	Mat ref = movingFrame->clone();		//基準画像を入手(movingFrameに入ってる必要がある)
//	do {
//		*input >> *movingFrame;
//	} while (!IsMovingFrame(movingFrame, &ref, mask));	//マスク画像を入手するためIsStartでmask指定
//}






/*手が変わる瞬間のフレームを探す*/
void SearchMoving(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame) {
	Mat ref = movingFrame->clone();		//基準画像を入手(movingFrameに入ってる必要がある)
	do {
		*input >> *movingFrame;
	} while (!IsMovingFrame(movingFrame, &ref, mask));	//マスク画像を入手するためIsStartでmask指定
}

/*基準画像とマスク画像から次の手になったかを判断する*/
bool IsMovingFrame(Mat *input, Mat *ref, MaskImageInfo *mask) {
	Mat img1, img2;
	bool flag = true;
	//1pのネクスト1,2、ネクネク1,2を基準と現在フレームでえる　順次見ていき全て変わったときのみOKとする
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			mask->GetMaskImageCircle(&img1, 0, i, j, input);	//一つずつ
			mask->GetMaskImageCircle(&img2, 0, i, j, ref);
			//img1とimg2の各ピクセルごとの誤差を記録する
			int error = Caluculate2ImageError(&img1, &img2);
			if (error < EOL) flag = false;
		}
	}
	if (flag)return true;
	return false;
}


bool SkipToNextGameStartFrame(cv::VideoCapture *video, MaskImageInfo *mask, cv::Mat *nowFrame) {
	int lastFrameNum = (int)video->get(CV_CAP_PROP_FRAME_COUNT);
	//はじめに試合画面を探す(スタートまでループ)
	do {
		*video >> *nowFrame;
		if (video->get(CV_CAP_PROP_POS_FRAMES) > lastFrameNum - 30) {
			cout << "スタート箇所が見つかりませんでした" << endl;
			return false;
		}
	} while (!mask->IsStartFrame(nowFrame));		//マスク画像を入手するためIsStartでmask指定

	FowardVideoFrame(video, nowFrame, 10);	//変更の余地あり
	return true;
}


//ぷよ消しorおじゃまふる前を探す	初めのフレーム番号に戻して　　ぷよ消し→おじゃま落下　　最後に今のフレーム番号に戻しておく
void SearchPuyoJustSetted(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame, int startNum, int endNum) {
	//スタートフレームに戻す＆movingFrameをそのフレームに戻す
	input->set(CV_CAP_PROP_POS_FRAMES, startNum);
	*input >> *movingFrame;

	//連鎖が起きたかチェック
	if (IsChain(input, mask, movingFrame, startNum, endNum)) {
		//連鎖起きてるならcapを最初のフレームに戻してreturn
		input->set(CV_CAP_PROP_POS_FRAMES, endNum);
		return;
	}

	//連鎖が起きていないならお邪魔落下するまでチェック
	//スタートフレームに戻す＆movingFrameをそのフレームに戻す
	input->set(CV_CAP_PROP_POS_FRAMES, startNum);
	*input >> *movingFrame;

	SearchBeforeOjamaDropFrame(input, mask, movingFrame, startNum, endNum);
	//capを最初のフレームに戻してreturn
	input->set(CV_CAP_PROP_POS_FRAMES, endNum);
}


//ネクストの動きが止まるまでcapをすすめる
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
		mask->GetMaskImageCircle(&img[0], 0, 2, 2, nowFrame);	//全範囲
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
	//	double mean[3] = { 0 }, dis[3] = { 0 };	//平均と分散
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
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//平均と分散
		//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
		//	Color col = PuyoDiscriminate(mean, dis);
			Color col = PuyoDiscriminate(img);

			data->InputNext(i, j, col);
			if (isCollectData) SaveOnePuyoImage(&img, col);
		}
	}
}

//fix?　時系列入れて、ロバストにする？
void DiscriminateField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	//まずは1pのフィールドだけ  ネクストのほう(Next)は変化してるとこ使ったらあかんことに注意
	Mat img, diffImage;
	mask->GetSrcDifferentImage(&diffImage, 0, frame);
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 12; y++) {
			((MaskImageInfo)(*mask)).GetMaskImageField(&img, 0, x, y, &diffImage);	//現在フレームの画像
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//平均と分散
		//	GetRGBMeanAndStandardDeviation(&img, mean, dis);
		//	Color col = PuyoDiscriminate(mean, dis);
			Color col = PuyoDiscriminate(img);
			data->InputField(x, y, col);

			if(isCollectData) SaveOnePuyoImage(&img,col);
		}
	}
}

void DiscriminateNextField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data) {
	//まずは1pのフィールドだけ  ネクストのほう(Next)は変化してるとこ使ったらあかんことに注意
	Mat img, diffImage;
	mask->GetSrcDifferentImage(&diffImage, 0, frame);
	for (int x = 0; x < 6; x++) {
		for (int y = 0; y < 12; y++) {
			((MaskImageInfo)(*mask)).GetMaskImageField(&img, 0, x, y, &diffImage);	//現在フレームの画像
		//	double mean[3] = { 0 }, dis[3] = { 0 };	//平均と分散
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
				vote[0]--;	//黒
			}
	//		else if (g > 120 && g > r + 30 && r > b + 5 && h > 60 && h < 100) {
			else if (g > 120 && g > r + 30 && r > b + 5) {
				vote[2]--;	//緑
			}
	//		else if (b > g + 40 && g > r + 40 && h > 120 && h < 150) {
			else if (b > g + 40 && g > r + 40) {
				vote[3]--;	//青
			}
	//		else if (abs(b - r) < 45 && r > g + 40 && h > 170 && h < 200) {
			else if (abs(b - r) < 45 && r > g + 40) {
				vote[5]--;	//紫
			}
	//		else if (r > 200 && r > g + 40 && abs(g - b) < 40 && (h > 220 || h < 10)) {
			else if (r > 200 && r > g + 40 && abs(g - b) < 40) {
				vote[1]--;	//赤
			}
	//		else if (r > 200 && g > b + 40 && h > 20 && h < 40 ) {
			else if (r > 200 && g > b + 40) {
				vote[4]--;	//黄
			}
			else if (abs(r - g) < 14 && abs(b - r) < 14 && abs(g - b) < 14) {
				vote[6]--;	//お邪魔
			}
			else {
				vote[7]--;
			} //不明
		}
	}
	if (-vote[0] > img.rows*img.cols*0.5) vote[0] = -img.rows*img.cols;//黒
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
//赤 R231  G=B   R>G=B
//青 R29-100   G88-150      B215-233        R>G>B
//黄 R255  G130-255  B80-128  R=255 G>B
//緑 G>R>B
//紫 R=B>G   B>R>>G
//お邪魔 R=G=B
}






////6次元ベクトルで判別する
//Color PuyoDiscriminate(double mean[3], double dis[3]) {
//	double input[6];
//	for (int j = 0; j < 3; j++)input[j] = mean[j];
//	for (int j = 0; j < 3; j++)input[j + 3] = dis[j];
//
//	//重心をゲットする
//	//重心入れる配列
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


//	//距離最短を探す
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
//		//RGをわける
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[0][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return P;
//		else return R;
//	case 2:
//		//GBをわける
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[1][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return G;
//		else return B;
//	case 3:
//		//YOをわける
//		for (int j = 0; j < 2; j++) secondDistance[j] = GetDistance(input, secondGp[2][j], 6);
//		SearchMinimum(secondDistance, 2, minValue2, min2);
//		if (min2 == 0)return Y;
//		else return O;
//	}
//	return N;
//}




























//連鎖起きているかチェック  得点が動いているかで判別
bool IsChain(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum) {
	int c = 0, th = 10; const int A = 1;
	Mat prev[A], img[A + 1];
	for (int i = 0; i < A - 1; i++) {
		prev[i] = nowFrame->clone();
		FowardVideoFrame(input, nowFrame, 1);
	}

	//無限に探索するのを防ぐため
	while (endNum - startNum + 1 > c) {
		c++;
		int error = 0;

		for (int i = A - 1; i > 0; i--) {
			prev[i] = prev[i - 1].clone();
		}
		prev[0] = nowFrame->clone();
		*input >> *nowFrame;

		mask->GetMaskImagePoint(&img[0], 0, nowFrame);	//得点の左側みる
		for (int i = 1; i < A + 1; i++) {
			mask->GetMaskImagePoint(&img[i], 0, &prev[i - 1]);
		}

		//誤差を計算する
		for (int i = 1; i < A + 1; i++) {
			error += Caluculate2ImageError(&img[0], &img[i]);
		}
		//	printf("%d\n", error);

		if (error > th) {
			int n = (int)(input->get(CV_CAP_PROP_POS_FRAMES) - A - 3);		//3は調整する
			input->set(CV_CAP_PROP_POS_FRAMES, n);
			*input >> *nowFrame;
			return true;
		}
	}
	return false;
}




//お邪魔が降る前のフレームにセット
void SearchBeforeOjamaDropFrame(cv::VideoCapture *input, MaskImageInfo *mask, Mat *nowFrame, int startNum, int endNum) {
	//今はendNumから？フレーム前のフレームにしとこう
	input->set(CV_CAP_PROP_POS_FRAMES, endNum - 5);
	*input >> *nowFrame;
}

