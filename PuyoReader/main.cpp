#include "main.h"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {

	//�Ղ效��ۑ�����f�B���N�g����
	string puyofuDirectoryName("puyofu\\");
	printf("�Ղ效��ۑ�����f�B���N�g�����F%s\n", puyofuDirectoryName.c_str());
	Directory::MakeDirectory(puyofuDirectoryName.c_str());

	//�f�B���N�g������t�@�C�����X�g�𒊏o
	string videoDirectoryName("videos\\");
	printf("���悪�����Ă���f�B���N�g�����F%s\n", videoDirectoryName.c_str());
	vector<string> videoFileNameList = Directory::GetFileNameList(videoDirectoryName, ".mp4");

	//�f�B���N�g������mp4������L�^���Ă���
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
			printf("���łɃt�H���_������̂Ŕ�΂��܂�\n");
		//	waitKey(0);
		}
	}


	// �Ղ效���Đ�����Ȃ炱����
	//	OneVideoPuyoPlayer play(fileName);
	//	play.Play();
	
	return 1;
}



