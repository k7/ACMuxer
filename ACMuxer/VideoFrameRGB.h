#pragma once
#include "VideoFrame.h"

class VideoFrameRGB : VideoFrame {
public:
	VideoFrameRGB(VideoFrame& videoFrame);
	AVPicture * getAVPicture();
	void SavePPM(string filename);

private:
	void fillRGB(VideoFrame& videoFrame);

	AVPixelFormat avPixelFormat = PIX_FMT_RGB24;
	struct SwsContext * swsContext = nullptr;
};

