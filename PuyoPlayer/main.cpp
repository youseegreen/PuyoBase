#include "OneVideoPuyoPlayer.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {

	cout << "再生するぷよ譜のフォルダ名を入力して下さい" << endl;
	cout << "フォルダ名：puyofu\\";
	string directoryName = "";
	cin >> directoryName;

	OneVideoPuyoPlayer play("puyofu\\" + directoryName + "\\");
	play.Play();

	return 1;
}



