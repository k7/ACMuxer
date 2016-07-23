#include "VideoStream.h"
#include "ContratException.h"
#include <iostream>

VideoStream::VideoStream(const unique_ptr<AVFormatContext, sFormatContextDeleter>& avFormatContext, int streamIndex) {
	
	// Get a pointer to the codec context for the video stream
	//AVCodecContext * pStreamCodecContext = avFormatContext->streams[streamIndex]->codec;
	AVCodecParameters * pStreamCodecParameters = avFormatContext->streams[streamIndex]->codecpar;
	
	// Find the decoder for the video stream
	AVCodec * pCodec = avcodec_find_decoder(pStreamCodecParameters->codec_id);
	ASSERTION(pCodec > nullptr); // Unsupported codec
	
	// Initialize the AVCodecContext to use the given AVCodec. The resulting struct should be freed with avcodec_free_context().
	//avCodecContext = shared_ptr<AVCodecContext>(avcodec_alloc_context3(pCodec), AVCodecContextDeleter);
	avCodecContext.reset(avcodec_alloc_context3(pCodec));
	
	// Initialize the AVCodecContext to use the given AVCodec. 
	int ret = avcodec_open2(avCodecContext.get(), pCodec, nullptr);
}

//VideoStream::~VideoStream() {
//	cout << "~VideoStream" << endl;
//	// Free the codec context and everything associated with it and write NULL to the provided pointer. 
//	avcodec_free_context(&avCodecContext);
//	//avcodec_free_context(&avCodecContext);
//	//avcodec_free_context(&avCodecContext);
//}
