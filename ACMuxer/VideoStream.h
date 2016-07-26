#pragma once
#include "ffmpeg.h"
#include <ostream>
#include <iostream>
#include <memory>
#include "Frame.h"

using namespace std;

class VideoStream
{
public:
	VideoStream(VideoStream&& other)
		: avCodecContext(std::move(other.avCodecContext)), avFormatContext(std::move(other).avFormatContext) {
		cout << "VideoStream:Move Constructor" << endl;
	}

	VideoStream& operator=(VideoStream&& other) {
		cout << "VideoStream:Move Assignment" << endl;
		if (this == &other)
			return *this;
		avCodecContext = std::move(other.avCodecContext);
		return *this;
	}

	VideoStream(const unique_ptr<AVFormatContext, AVFormatContextDeleter> &avFormatContext, int streamIndex);
	unique_ptr<AVCodecContext, AVCodecContextDeleter> avCodecContext;
	const unique_ptr<AVFormatContext, AVFormatContextDeleter>& avFormatContext;
	Frame getNextFrame();
	
private:
	int pc = 0;
	AVPacket _pkt;
	unique_ptr<AVPacket, AVPacketDeleter> pkt; // Wrap _pkt to ensures last unref on destruction
	int state = AVERROR(EAGAIN); // Start state = needs more input
	int readPacket();
	bool eof = false;
};

