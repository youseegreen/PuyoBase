#pragma once
//https://tanjoin.hatenablog.com/entry/20110810/1312966845

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <Windows.h>
#include <direct.h>
#include <regex>

using namespace std;

class Directory
{
public:
	// コンストラクタ
	Directory(void) {}
	// デストラクタ
	virtual ~Directory(void) {}




	// ファイル一覧取得
	// folder : フォルダの絶対パスを入力とする 
	// 例 : "D:\\Users\\Pictures\\"
	static vector<string> GetDirectoryNameList(string folder, regex reg) {
		// 宣言
		vector<string> folderList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// ファイル名検索のためにワイルドカード追加
		// 例 : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*";

		// ファイル探索
		// FindFirstFile(ファイル名, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// 検索失敗
		if (hFind == INVALID_HANDLE_VALUE) {
			std::cout << "フォルダ一覧を取得できませんでした" << std::endl;
			exit(1); // エラー終了
		}

		// ファイル名をリストに格納するためのループ
		do {
			// フォルダは除く
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//ファイル名をリストに格納
				char *folder = fd.cFileName;
				string str = folder;

				//fix
				smatch match;
				if (regex_match(str, match, reg)) {
					folderList.push_back(str);
				}
			}
		} while (FindNextFile(hFind, &fd)); //次のファイルを探索

											// hFindのクローズ
		FindClose(hFind);
		return folderList;
	}






	// ファイル一覧取得
	// folder : フォルダの絶対パスを入力とする 
	// 例 : "D:\\Users\\Pictures\\"
	static vector<string> GetFileNameList(string folder, string fileExtension = ".*") {
		// 宣言
		vector<string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// ファイル名検索のためにワイルドカード追加
		// 例 : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*";
		ss << fileExtension;

		// ファイル探索
		// FindFirstFile(ファイル名, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// 検索失敗
		if (hFind == INVALID_HANDLE_VALUE) {
			std::cout << "ファイル一覧を取得できませんでした" << std::endl;
			exit(1); // エラー終了
		}

		// ファイル名をリストに格納するためのループ
		do {
			// フォルダは除く
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				//ファイル名をリストに格納
				char *file = fd.cFileName;
				string str = file;
				fileList.push_back(str);
			}
		} while (FindNextFile(hFind, &fd)); //次のファイルを探索

											// hFindのクローズ
		FindClose(hFind);

		return fileList;
	}

	//フォルダがなければ作成する
	static bool MakeDirectory(const char * directoryName) {
		if (_mkdir(directoryName) == 0)return true;
		else return false;
	}
};
