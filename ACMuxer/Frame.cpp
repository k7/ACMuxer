﻿#include "Frame.h"
// Access options?
//const AVClass* c = avcodec_get_frame_class();
//AVFrame* f = frame.avFrame.get();
//c = reinterpret_cast<const AVClass*>(f);
int64_t Frame::getRelativePts() const {
	return pkt.pts - firstFramePts;
}
