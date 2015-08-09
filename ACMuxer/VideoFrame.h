#pragma once
#include "ffmpeg.h"
#include "ContratException.h"

using namespace std;

class VideoFrame {
public:
	VideoFrame(AVCodecContext * codecContext);
	~VideoFrame();

private:
	AVCodecContext * pCodecContext;
	int streamIndex;
};