#include "main.h"

using namespace cv;
using namespace std;



int main(int argc, char *argv[]) {

//	GetPUYOHUFromOneVideo("videos\\maha-ra-makki.mp4");	

//	OneVideoPuyoPlayer play("maha-ra-makki");
//	play.Play();

	//�f�B���N�g������t�@�C�����X�g�𒊏o
//	waitKey(0);
	printf("���悪�����Ă���f�B���N�g����\n");
	string directoryName("videos\\");
	vector<string> videoFileNameList = Directory::GetFileNameList(directoryName, ".mp4");

	//�f�B���N�g������mp4������L�^���Ă���
	for (int i = 0; i < videoFileNameList.size(); i++) {
		//���łɃt�H���_�����Ƃ΂��Ƃ�������
		cout << videoFileNameList[i] << endl;

		string fileNameOnly = videoFileNameList[i].substr(videoFileNameList[i].find_last_of("\\") + 1,
			videoFileNameList[i].find_last_of(".") - videoFileNameList[i].find_last_of("\\") - 1);
		if (Directory::MakeDirectory(fileNameOnly.c_str())) {
			string fName = directoryName + videoFileNameList[i];
			GetPUYOHUFromOneVideo(fName.c_str());
		}
		else {
			printf("���łɃt�H���_������̂Ŕ�΂��܂�\n");
		//	waitKey(0);
		}
	}
	
	return 1;
}



