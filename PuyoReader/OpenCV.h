#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/flann.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/photo.hpp>
#include <opencv2/shape.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/superres.hpp>
#include <opencv2/video.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/videostab.hpp>


// �o�[�W�������̎擾
#define CV_VERSION_STR CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)

// lib�t�@�C�����̍Ō�̕�����Release��Debug�ŕ�����
#ifdef _DEBUG
#define CV_EXT_STR "d.lib"
#else
#define CV_EXT_STR ".lib"
#endif

//�v���W�F�N�g�̃v���p�e�B�˃����J�[�ˑS�ʁ@�̒ǉ��̃��C�u�����f�B���N�g����
// lib�t�@�C���̂���t�H���_�iC:\opencv\build\x86\vc10\lib�Ȃǁj��ǉ��̂���
#pragma comment(lib, "opencv_world"  CV_VERSION_STR CV_EXT_STR)