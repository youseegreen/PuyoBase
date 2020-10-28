#pragma once
#include <iostream>
#include "GetFileNameFromFolder.h"
#include "MaskImageInfo.h"
#include "DataManager.h"

void GetPUYOHUFromOneVideo(const string videoFileName, const string saveDirectoryName);


//Ç‡ÇµÇ©ÇµÇΩÇÁÅAcppÇæÇØÇ…Ç∑ÇÈÇ©Ç‡
bool SkipToNextGameStartFrame(cv::VideoCapture *video, MaskImageInfo *mask, cv::Mat *nowFrame);
void SearchMoving(VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame);
void SearchPuyoJustSetted(cv::VideoCapture *input, MaskImageInfo *mask, Mat *movingFrame, int startNum, int endNum);
void FowardFrameUntillDontMove(cv::VideoCapture *input, MaskImageInfo *mask, cv::Mat *nowFrame);
void DiscriminateNextField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data);

void DiscriminateField(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data);
void DiscriminateNext(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data);
void DiscriminateNow(const cv::Mat *frame, MaskImageInfo* const mask, DataInputor *data);