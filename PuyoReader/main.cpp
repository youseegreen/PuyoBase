#include "main.h"

using namespace cv;
using namespace std;



int main(int argc, char *argv[]) {

//	GetPUYOHUFromOneVideo("videos\\maha-ra-makki.mp4");	

//	OneVideoPuyoPlayer play("maha-ra-makki");
//	play.Play();

	//ディレクトリからファイルリストを抽出
//	waitKey(0);
	printf("動画が入っているディレクトリ名\n");
	string directoryName("videos\\");
	vector<string> videoFileNameList = Directory::GetFileNameList(directoryName, ".mp4");

	//ディレクトリ内のmp4を一つずつ記録していく
	for (int i = 0; i < videoFileNameList.size(); i++) {
		//すでにフォルダあるやつとばすとかしたい
		cout << videoFileNameList[i] << endl;

		string fileNameOnly = videoFileNameList[i].substr(videoFileNameList[i].find_last_of("\\") + 1,
			videoFileNameList[i].find_last_of(".") - videoFileNameList[i].find_last_of("\\") - 1);
		if (Directory::MakeDirectory(fileNameOnly.c_str())) {
			string fName = directoryName + videoFileNameList[i];
			GetPUYOHUFromOneVideo(fName.c_str());
		}
		else {
			printf("すでにフォルダがあるので飛ばします\n");
		//	waitKey(0);
		}
	}
	
	return 1;
}



