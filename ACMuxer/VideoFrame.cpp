#include "VideoFrame.h"

VideoFrame::VideoFrame(AVCodecContext * avCodecContext):avCodecContext(avCodecContext){
	PRECONDITION(avCodecContext != nullptr);
	avFrame = av_frame_alloc();
	POSTCONDITION(avFrame != nullptr);
}

// Move constructor
// https://msdn.microsoft.com/en-us/library/dd293665.aspx
VideoFrame::VideoFrame(VideoFrame&& other) {
	avCodecContext = other.avCodecContext;
	avFrame = other.avFrame;
	
	// Free other object so destructor does not free it
	other.avCodecContext = nullptr;
	other.avFrame = nullptr;
}

// Move assignment operator
// https://msdn.microsoft.com/en-us/library/dd293665.aspx
VideoFrame& VideoFrame::operator=(VideoFrame&& other) {
	if(this != &other) {
		// Free existing resources
		freeResources();
		// Move from other object
		avFrame = other.avFrame;
		// Free other object so destructor does not free it
		other.avFrame = nullptr;
	}
	return *this;
}

VideoFrame::~VideoFrame()
{
	freeResources();
}


void VideoFrame::freeResources() {
	if (avFrame) {
		av_frame_free(&avFrame);
		avFrame = nullptr;
	}
}

AVCodecContext* VideoFrame::getAVCodecContext() const {
	return avCodecContext;
}

AVFrame* VideoFrame::getAVFrame() const {
	return avFrame;
}

void VideoFrame::reset(AVFrame* avFrame) {
	PRECONDITION(avFrame != nullptr);
	
	// free any references held by the frame and reset it to its original clean state before it is reused again.
	av_frame_unref(avFrame);

}

int VideoFrame::getWidth() const {
	return avCodecContext->width;
}

int VideoFrame::getHeight() const {
	return avCodecContext->height;
}

AVPixelFormat VideoFrame::getAVPixelFormat() const {
	return avCodecContext->pix_fmt;
}