#include "OneVideoPuyoPlayer.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {

	cout << "�Đ�����Ղ效�̃t�H���_������͂��ĉ�����" << endl;
	cout << "�t�H���_���Fpuyofu\\";
	string directoryName = "";
	cin >> directoryName;

	OneVideoPuyoPlayer play("puyofu\\" + directoryName + "\\");
	play.Play();

	return 1;
}



