#pragma once
#include "ffmpeg.h"
#include "VideoFrame.h"

/*
AVPicture (RGB24)
https://www.ffmpeg.org/doxygen/2.6/structAVPicture.html
*/
class Picture : VideoFrame
{
public:
	Picture() = delete;
	Picture(VideoFrame& videoFrame);
	Picture(const Picture& other) = delete;				// Disable copy constructor
	Picture& operator=(const Picture& other) = delete;	// Disable copy assignment
	//Picture(Picture&& other);							// Move constructor
	//Picture& operator=(Picture&& other);				// Move assignment
	~Picture();

	void freeResources();
	void SavePPM(const string& filename);
private:
	const AVPixelFormat avPixelFormat = PIX_FMT_RGB24;
	struct SwsContext * swsContext = nullptr;
};

