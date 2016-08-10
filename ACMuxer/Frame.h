#pragma once
#include "ffmpeg.h"


class Frame {
public:
  	//Frame(unique_ptr<AVFrame, AVFrameDeleter> frame) : avFrame(std::move(frame)) {}
	Frame(unique_ptr<AVFrame, AVFrameDeleter> frame, const AVPacket& pkt) : avFrame(std::move(frame)) {
	   av_packet_copy_props(&this->pkt, &pkt); // Copy non-data fields
	}
	bool operator!() const {
		return !(avFrame && avFrame->width > 0);
	}
	
	int64_t getBestEffortTimeStamp() const {
	  return av_frame_get_best_effort_timestamp(this->avFrame.get());
	}; // so far, equal to pts

	int64_t getRelativePts() const;

	unique_ptr<AVFrame, AVFrameDeleter> avFrame;
	AVPacket pkt; // copy of pkt fields for this frame
	int64_t firstFramePts = -1;
private:
	
};



