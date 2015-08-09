#include "ffmpeg.h"
#include <string>
#include <vector>
#include <memory>
#include <iostream>

using namespace std;

class InputVideo{
public:
	InputVideo(string filepath);
	~InputVideo();
	int getStreamCount();
	vector<int> getVideoStreamIndices();
	AVCodecContext * getCodecContext(int streamIndex);
	AVFrame * getNextFrame();
	//unique_ptr<AVFrame, AVPtrDeleter> getNextFrame();
	void DisplayFileInfo();

private:
	AVDictionary ** pDictionary = nullptr; // Options, unused
	AVFormatContext * pFormatContext = nullptr;
	AVInputFormat * pInputFormat = nullptr; // Input Format
	AVCodecContext *pCodecContext = nullptr; // Codec Context
	int currentStreamIndex = -1;
	int currentFrameIndex = -1;
	InputVideo(const InputVideo &) = delete;
	string filename;
	
};