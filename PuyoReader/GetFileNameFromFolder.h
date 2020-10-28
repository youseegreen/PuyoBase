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
	// �R���X�g���N�^
	Directory(void) {}
	// �f�X�g���N�^
	virtual ~Directory(void) {}




	// �t�@�C���ꗗ�擾
	// folder : �t�H���_�̐�΃p�X����͂Ƃ��� 
	// �� : "D:\\Users\\Pictures\\"
	static vector<string> GetDirectoryNameList(string folder, regex reg) {
		// �錾
		vector<string> folderList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// �t�@�C���������̂��߂Ƀ��C���h�J�[�h�ǉ�
		// �� : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*";

		// �t�@�C���T��
		// FindFirstFile(�t�@�C����, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// �������s
		if (hFind == INVALID_HANDLE_VALUE) {
			std::cout << "�t�H���_�ꗗ���擾�ł��܂���ł���" << std::endl;
			exit(1); // �G���[�I��
		}

		// �t�@�C���������X�g�Ɋi�[���邽�߂̃��[�v
		do {
			// �t�H���_�͏���
			if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				//�t�@�C���������X�g�Ɋi�[
				char *folder = fd.cFileName;
				string str = folder;

				//fix
				smatch match;
				if (regex_match(str, match, reg)) {
					folderList.push_back(str);
				}
			}
		} while (FindNextFile(hFind, &fd)); //���̃t�@�C����T��

											// hFind�̃N���[�Y
		FindClose(hFind);
		return folderList;
	}






	// �t�@�C���ꗗ�擾
	// folder : �t�H���_�̐�΃p�X����͂Ƃ��� 
	// �� : "D:\\Users\\Pictures\\"
	static vector<string> GetFileNameList(string folder, string fileExtension = ".*") {
		// �錾
		vector<string> fileList;
		HANDLE hFind;
		WIN32_FIND_DATA fd;

		// �t�@�C���������̂��߂Ƀ��C���h�J�[�h�ǉ�
		// �� : "D:\\Users\\Pictures\\*.*"
		stringstream ss;
		ss << folder;
		string::iterator itr = folder.end();
		itr--;
		if (*itr != '\\') ss << '\\';
		ss << "*";
		ss << fileExtension;

		// �t�@�C���T��
		// FindFirstFile(�t�@�C����, &fd);
		hFind = FindFirstFile(ss.str().c_str(), &fd);

		// �������s
		if (hFind == INVALID_HANDLE_VALUE) {
			std::cout << "�t�@�C���ꗗ���擾�ł��܂���ł���" << std::endl;
			exit(1); // �G���[�I��
		}

		// �t�@�C���������X�g�Ɋi�[���邽�߂̃��[�v
		do {
			// �t�H���_�͏���
			if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				&& !(fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN))
			{
				//�t�@�C���������X�g�Ɋi�[
				char *file = fd.cFileName;
				string str = file;
				fileList.push_back(str);
			}
		} while (FindNextFile(hFind, &fd)); //���̃t�@�C����T��

											// hFind�̃N���[�Y
		FindClose(hFind);

		return fileList;
	}

	//�t�H���_���Ȃ���΍쐬����
	static bool MakeDirectory(const char * directoryName) {
		if (_mkdir(directoryName) == 0)return true;
		else return false;
	}
};
