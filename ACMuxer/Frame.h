#pragma once
#include "ffmpeg.h"


class Frame {
public:
  	//Frame(unique_ptr<AVFrame, AVFrameDeleter> frame) : avFrame(std::move(frame)) {}
	Frame(unique_ptr<AVFrame, AVFrameDeleter> frame, const AVPacket& pkt) : avFrame(std::move(frame)) {
	   av_packet_copy_props(&this->pkt, &pkt); // Copy non-data fields
	}
	bool operator!() const {
		return !avFrame;
	}
	
	unique_ptr<AVFrame, AVFrameDeleter> avFrame;
	AVPacket pkt; // copy of pkt fields for this frame
private:
	
};



