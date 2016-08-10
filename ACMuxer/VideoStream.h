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
	VideoStream(const unique_ptr<AVFormatContext, AVFormatContextDeleter> &avFormatContext, int streamIndex);

	unique_ptr<AVCodecContext, AVCodecContextDeleter> avCodecContext;
	const unique_ptr<AVFormatContext, AVFormatContextDeleter>& avFormatContext;
	Frame getNextFrame();
	
	// Accessors
	AVStream * getAVStream() const { return avFormatContext.get()->streams[streamIndex]; }
	AVCodecParameters * getAVCodecParameters() const {return avFormatContext.get()->streams[streamIndex]->codecpar;}
	AVRational * getStreamTimeBase() const {
	  return &avFormatContext.get()->streams[streamIndex]->time_base;
	};
		
private:
	int readPacket();
	string getErrorReadPacket(int ret);
	AVPacket _pkt;
	unique_ptr<AVPacket, AVPacketDeleter> pkt; // Wrap _pkt to ensures last unref on destruction

	bool eof = false;
	int state = AVERROR(EAGAIN); // Initial state = needs more input
	int64_t firstFramePts = -1;  // To offset later frames

	int streamIndex;
	
};

