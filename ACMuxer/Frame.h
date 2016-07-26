#pragma once
#include "ffmpeg.h"

class Frame {
public:

	Frame(unique_ptr<AVFrame, AVFrameDeleter> frame)
		: avFrame(std::move(frame)) {
	}
	bool operator!() const {
		return !avFrame;
	}
private:
	unique_ptr<AVFrame, AVFrameDeleter> avFrame;
};
