#pragma once
#include "ffmpeg.h"
#include <ostream>
#include <iostream>
#include <memory>
using namespace std;

class VideoStream
{
public:
	//VideoStream(const VideoStream& other) = delete;
	//VideoStream& operator=(const VideoStream& other) = delete;

	VideoStream(VideoStream&& other)
		: avCodecContext(std::move(other.avCodecContext)) {
		cout << "VideoStream:Move Constructor" << endl;
	}

	VideoStream& operator=(VideoStream&& other) {
		cout << "VideoStream:Move Assignment" << endl;
		if (this == &other)
			return *this;
		avCodecContext = std::move(other.avCodecContext);
		return *this;
	}

	VideoStream(const unique_ptr<AVFormatContext, sFormatContextDeleter> &avFormatContext, int streamIndex);

private:
	unique_ptr<AVCodecContext, sCodecContextDeleter> avCodecContext;

};

