#include "main.h"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	//ぷよ譜を保存するディレクトリ名
	string puyofuDirectoryName("puyofu\\");
	printf("ぷよ譜を保存するディレクトリ名：%s\n", puyofuDirectoryName.c_str());
	Directory::MakeDirectory(puyofuDirectoryName.c_str());

	//ディレクトリからファイルリストを抽出
	string videoDirectoryName("videos\\");
	printf("動画が入っているディレクトリ名：%s\n", videoDirectoryName.c_str());
	vector<string> videoFileNameList = Directory::GetFileNameList(videoDirectoryName, ".mp4");

	//ディレクトリ内のmp4を一つずつ記録していく
	for (int i = 0; i < videoFileNameList.size(); i++) {
		cout << videoFileNameList[i] << endl;

		string fileNameOnly = videoFileNameList[i].substr(videoFileNameList[i].find_last_of("\\") + 1,
			videoFileNameList[i].find_last_of(".") - videoFileNameList[i].find_last_of("\\") - 1);

		if (Directory::MakeDirectory((puyofuDirectoryName + fileNameOnly).c_str())) {
			string videoFileName = videoDirectoryName + videoFileNameList[i];
			string saveDirectoryName = puyofuDirectoryName + fileNameOnly + string("\\");
			GetPUYOHUFromOneVideo(videoFileName, saveDirectoryName);
		}
		else {
			printf("すでにフォルダがあるので飛ばします\n");
		//	waitKey(0);
		}
	}


	// ぷよ譜を再生するならこちら
	//	OneVideoPuyoPlayer play(fileName);
	//	play.Play();
	
	return 1;
}



