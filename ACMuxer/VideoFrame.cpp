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
		// Free other object
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

AVCodecContext* VideoFrame::getAVCodecContext(){
	return avCodecContext;
}

AVFrame* VideoFrame::getAVFrame(){
	return avFrame;
}

void VideoFrame::reset(AVFrame* avFrame){
	PRECONDITION(avFrame != nullptr);
	
	// free any references held by the frame and reset it to its original clean state before it is reused again.
	av_frame_unref(avFrame);

}

int VideoFrame::getWidth() {
	return avCodecContext->width;
}

int VideoFrame::getHeight() {
	return avCodecContext->height;
}

AVPixelFormat VideoFrame::getAVPixelFormat() {
	return avCodecContext->pix_fmt;
}